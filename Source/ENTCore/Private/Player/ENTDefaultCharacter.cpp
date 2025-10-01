// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/ENTDefaultCharacter.h"
#include "AkComponent.h"
#include "Interface/ENTGroundAction.h"
#include "Camera/CameraComponent.h"
#include "ENTCameraShakeComponent.h"
#include "ENTHealthComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "ENTInteractableComponent.h"
#include "ENTToolStatics.h"
#include "Components/PostProcessComponent.h"
#include "GameElements/ENTAmberOre.h"
#include "GameElements/ENTRespawnTree.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterState.h"
#include "Player/States/ENTCharacterStateMachine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/AIModule/Classes/Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Config/ENTCoreConfig.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Player/States/ENTCharacterFallState.h"
#include "Saves/ENTPlayerSave.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTWorldSave.h"
#include "Subsystems/ENTPlayerSaveSubsystem.h"

AENTDefaultCharacter::AENTDefaultCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f));
	CameraComponent->bUsePawnControlRotation = true;

	PostProcessComp = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	PostProcessComp->SetupAttachment(CameraComponent);
	PostProcessComp->bUnbound = true;

	ShakeManager = CreateDefaultSubobject<UENTCameraShakeComponent>(TEXT("Shake Manager"));

	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	CharacterMesh->SetOnlyOwnerSee(true);
	CharacterMesh->SetupAttachment(CameraComponent);
	CharacterMesh->bCastDynamicShadow = false;
	CharacterMesh->CastShadow = false;
	CharacterMesh->SetRelativeLocation(FVector(-30.0f, 0.0f, -150.0f));

	HearingStimuli = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("Hearing");
	HearingStimuli->bAutoRegister = true;
	HearingStimuli->RegisterForSense(UAISense_Hearing::StaticClass());

	SpikeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SpikeMesh"));
	SpikeMesh->SetupAttachment(CameraComponent);

	FootstepsSounds = CreateDefaultSubobject<UAkComponent>(TEXT("FootstepsSounds"));
	FootstepsSounds->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<UENTHealthComponent>("Health");

	AmberInventoryMaxCapacity.Add(EAmberType::NecroseAmber, 3);
	AmberInventoryMaxCapacity.Add(EAmberType::WeakAmber, 1);
}

void AENTDefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpikeRelativeTransform = SpikeMesh->GetRelativeTransform();
	SpikeTargetTransform = SpikeRelativeTransform;
	SpikeParent = SpikeMesh->GetAttachParent();

	AmberInventory.Add(EAmberType::NecroseAmber, 0);
	AmberInventory.Add(EAmberType::WeakAmber, 0);

	if (PostProcessComp && SpeedEffectMaterialReference)
	{
		SpeedEffectMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, SpeedEffectMaterialReference);
		PostProcessComp->Settings.AddBlendable(SpeedEffectMaterial, 1.0f);
	}

	if (HealthComponent)
	{
		HealthComponent->OnHealthNull.AddDynamic(this, &AENTDefaultCharacter::OnPlayerDie);
	}

	if (!GetController())
	{
		return;
	}

	AENTDefaultPlayerController* CastedController = Cast<AENTDefaultPlayerController>(GetController());
	if (!CastedController)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("PlayerController of %s is %s but should be %s"), *GetClass()->GetName(), *GetController()->GetClass()->GetName(), *AENTDefaultPlayerController::StaticClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
		return;
	}

	FirstPersonController = CastedController;

	const UENTCoreConfig* CoreConfig = GetDefault<UENTCoreConfig>();
	if (!CoreConfig)
	{
		return;
	}

	UCameraShakeBase* CameraShake = FirstPersonController->PlayerCameraManager->StartCameraShake(CoreConfig->ViewBobbingClass, 1.0f, ECameraShakePlaySpace::World);
	if (!CameraShake)
	{
		return;
	}

	UENTViewBobbing* CastedCameraShake = Cast<UENTViewBobbing>(CameraShake);
	if (!CastedCameraShake)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("ViewBobbingClass in %s is class of %s but should be %s"), *CoreConfig->GetClass()->GetName(), *CameraShake->GetClass()->GetName(), *UENTViewBobbing::StaticClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
		return;
	}

	ViewBobbing = CastedCameraShake;

	DefaultFootStepEvent = FootstepsSounds->AkAudioEvent;

	CreateStates();
	InitStateMachine();
}

void AENTDefaultCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (HealthComponent)
	{
		HealthComponent->OnHealthNull.RemoveDynamic(this, &AENTDefaultCharacter::OnPlayerDie);
	}

	OnRespawn.Clear();
	OnAmberUpdate.Clear();
	OnInteractionFeedback.Clear();
}

void AENTDefaultCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickStateMachine(DeltaSeconds);
	InteractionTrace();
	GroundMovement();
	UpdateSpikeLocation(DeltaSeconds);
	UpdateSpeedEffect(DeltaSeconds);

	if (CurrentInteractable && GetPlayerController()->GetPlayerInputs().bInputInteractPressed)
	{
		CurrentInteractable->Interact(GetPlayerController(), this);
	}
}

#pragma region StateMachine

void AENTDefaultCharacter::UpdateSpeedEffect(float DeltaSeconds)
{
	if (!SpeedEffectMaterialReference)
	{
		return;
	}

	float ParamValue;
	SpeedEffectMaterial->GetScalarParameterValue(SpeedEffectParamName, ParamValue);

	float NormalizeValue = UENTToolStatics::GetNormalizedFloatRange(GetVelocity().Length(), SpeedEffectVelocityRange);

	ParamValue = FMath::Lerp(ParamValue, NormalizeValue, DeltaSeconds);
	ParamValue = FMath::Clamp(ParamValue, 0.0f, 1.0f);

#if WITH_EDITORONLY_DATA
	if (bShowSpeedEffectValues)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("VelocityLength: %f"), GetVelocity().Length()));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("SpeedEffectParamValue: %f"), ParamValue));
	}
#endif

	SpeedEffectMaterial->SetScalarParameterValue(SpeedEffectParamName, ParamValue);
}

void AENTDefaultCharacter::InitStateMachine()
{
	StateMachine = NewObject<UENTCharacterStateMachine>();

	if (!StateMachine)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Failed to create StateMachine"));

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
		return;
	}

	StateMachine->InitStateMachine(this);
}

void AENTDefaultCharacter::TickStateMachine(float DeltaTime)
{
	if (!StateMachine)
	{
		return;
	}

	StateMachine->TickStateMachine(DeltaTime);
}

void AENTDefaultCharacter::CreateStates()
{
	for (const TTuple<EENTCharacterStateID, TSubclassOf<UENTCharacterState>>& State : CharacterStates)
	{
		if (State.Key == EENTCharacterStateID::None)
		{
#if WITH_EDITOR
			const FString Message = FString::Printf(TEXT("Cannot create state because stateID is None"));

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
			FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
			continue;
		}

		if (!State.Value)
		{
#if WITH_EDITOR
			const FString Message = FString::Printf(TEXT("Cannot create state because stateClass is nullptr"));

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
			FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
			continue;
		}

		UENTCharacterState* NewState = NewObject<UENTCharacterState>(this, State.Value);
		if (!NewState)
		{
#if WITH_EDITOR
			const FString Message = FString::Printf(TEXT("Failed to create state %d"), State.Key);

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
			FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
			continue;
		}

		States.Add(NewState);
	}
}

#if WITH_EDITOR
void AENTDefaultCharacter::DisplayStates(bool bDisplay)
{
	bDebugStates = bDisplay;
}
#endif

#pragma endregion

#pragma region Interaction

void AENTDefaultCharacter::InteractionTrace()
{
	const UENTCoreConfig* CoreConfig =  GetDefault<UENTCoreConfig>();

	if (!CoreConfig)
	{
		return;
	}

	FVector StartLocation = CameraComponent->GetComponentLocation();
	FVector EndLocation = (CameraComponent->GetForwardVector() * InteractionTraceLength) + StartLocation;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, CoreConfig->InteractionTraceChannel, CollisionParams);

	if (!bHit || !HitResult.GetActor())
	{
		RemoveInteraction();
		return;
	}

	UActorComponent* FoundComp = HitResult.GetActor()->GetComponentByClass(UENTInteractableComponent::StaticClass());
	if (!FoundComp)
	{
		RemoveInteraction();
		return;
	}

	UENTInteractableComponent* TargetInteractable = Cast<UENTInteractableComponent>(FoundComp);
	if (!TargetInteractable)
	{
		RemoveInteraction();
		return;
	}

	if (TargetInteractable->CheckComponent(HitResult.GetComponent()))
	{
		OnInteractionFeedback.Broadcast(true);
		CurrentInteractable = TargetInteractable;
		CurrentInteractable->SelectPrimitive(HitResult.GetComponent());

#if WITH_EDITORONLY_DATA
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("Interaction: %s"), *CurrentInteractable->GetInteractionName().ToString()));
#endif
	}
	else
	{
		RemoveInteraction();
	}
}

void AENTDefaultCharacter::RemoveInteraction()
{
	OnInteractionFeedback.Broadcast(false);
	if(!CurrentInteractable)
	{
		return;
	}

	CurrentInteractable->SelectPrimitive(nullptr);
	CurrentInteractable = nullptr;
}

#pragma endregion

#pragma region Ground

void AENTDefaultCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	AboveActor(Hit.GetActor());

	FHitResult HitResult;
	bool bHit = GroundTrace(HitResult);

	if (!bHit || !HitResult.PhysMaterial.IsValid())
	{
		return;
	}

	const UENTCoreConfig* CoreConfig = GetDefault<UENTCoreConfig>();
	if (!CoreConfig)
	{
		return;
	}

	const UAkSwitchValue* SurfaceNoise = CoreConfig->FindNoise(HitResult.PhysMaterial->SurfaceType);
	if (SurfaceNoise)
	{
		FootstepsSounds->SetSwitch(SurfaceNoise);
	}

	FootstepsSounds->PostAkEvent(LandedEvent);
	ResetFootStepsEvent();
}

bool AENTDefaultCharacter::GroundTrace(FHitResult& HitResult) const
{
	const UENTCoreConfig* CoreConfig =  GetDefault<UENTCoreConfig>();

	if (!CoreConfig)
	{
		return false;
	}

	FVector StartLocation = GetBottomLocation();
	FVector EndLocation = StartLocation;
	EndLocation.Z -= GroundTraceLength;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bReturnPhysicalMaterial = true;

	// const FCollisionObjectQueryParams ObjectParams = ConfigureCollisionObjectParams(CoreConfig->GroundObjectTypes);

	TArray<AActor*> ActorsToIgnore;

	return UKismetSystemLibrary::LineTraceSingleForObjects(this, StartLocation, EndLocation, CoreConfig->GroundObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	// return GetWorld()->LineTraceSingleByObjectType(HitResult, StartLocation, EndLocation, ObjectParams, CollisionQueryParams);
}

void AENTDefaultCharacter::GroundMovement()
{
	FHitResult Hit;
	if (!GetCharacterMovement()->IsFalling() && GroundTrace(Hit))
	{
		AboveActor(Hit.GetActor());
	}

	if (GetCharacterMovement()->IsFalling() && GroundActor)
	{
		GroundActor = nullptr;
	}
}

void AENTDefaultCharacter::AboveActor(AActor* ActorBellow)
{
	if (!ActorBellow || ActorBellow == GroundActor)
	{
		return;
	}

	GroundActor = ActorBellow;

	if (GroundActor->Implements<UENTGroundAction>())
	{
		IENTGroundAction::Execute_OnActorAbove(GroundActor, this);
	}
}

#pragma endregion

#pragma region Amber

void AENTDefaultCharacter::OnEnterWeakZone_Implementation(bool bIsZoneActive)
{
	IENTWeakZoneInterface::OnEnterWeakZone_Implementation(bIsZoneActive);
}

void AENTDefaultCharacter::OnExitWeakZone_Implementation()
{
	IENTWeakZoneInterface::OnExitWeakZone_Implementation();
}

void AENTDefaultCharacter::MineAmber(const EAmberType& AmberType, const int Amount)
{
	int* Count = AmberInventory.Find(AmberType);
	int* MaxCapacity = AmberInventoryMaxCapacity.Find(AmberType);

	// Count == nullptr means AmberType key doesn't exist
	if (!Count || !MaxCapacity)
	{
		return;
	}

	*Count += Amount;
	*Count = FMath::Clamp(*Count, 0.0f, *MaxCapacity);

	OnAmberUpdate.Broadcast(AmberType, *Count);

	UENTPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UENTPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem || !PlayerSaveSubsystem->GetPlayerSave())
	{
		return;
	}

	PlayerSaveSubsystem->GetPlayerSave()->AmberInventory.Empty(AmberInventory.Num());
	for (const TTuple<EAmberType, int>& Element : AmberInventory)
	{
		PlayerSaveSubsystem->GetPlayerSave()->AmberInventory.Add(static_cast<uint8>(Element.Key), Element.Value);
	}
}

void AENTDefaultCharacter::UseAmber(const EAmberType& AmberType, const int Amount)
{
	MineAmber(AmberType, -Amount);
}

bool AENTDefaultCharacter::IsAmberTypeFilled(const EAmberType& AmberType) const
{
	const int* Count = AmberInventory.Find(AmberType);
	const int* MaxCapacity = AmberInventoryMaxCapacity.Find(AmberType);

	if (!Count || !MaxCapacity)
	{
		return false;
	}

	return *Count == *MaxCapacity;
}

bool AENTDefaultCharacter::HasRequiredQuantity(const EAmberType& AmberType, const int Quantity) const
{
	const int* Count = AmberInventory.Find(AmberType);

	if (!Count)
	{
		return false;
	}

	return *Count >= Quantity;
}

#pragma endregion

#pragma region Spike

void AENTDefaultCharacter::PlantSpike(const FVector& TargetLocation)
{
	SpikeMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	bUseSpikeRelativeTransform = false;
	SpikeTargetTransform = FTransform(TargetLocation);
}

void AENTDefaultCharacter::ReGrabSpike()
{
	SpikeMesh->AttachToComponent(SpikeParent, FAttachmentTransformRules::KeepWorldTransform);
	bUseSpikeRelativeTransform = true;
	SpikeTargetTransform = SpikeRelativeTransform;
}

void AENTDefaultCharacter::UpdateSpikeOffset(float Alpha) const
{
	FVector DefaultLocation = SpikeRelativeTransform.GetLocation();
	FVector OffsetLocation = DefaultLocation - (FVector::ForwardVector * SpikeChargingOffset);
	FVector TargetLocation = UKismetMathLibrary::VLerp(DefaultLocation, OffsetLocation, Alpha);

	SpikeMesh->SetRelativeLocation(TargetLocation);
}

void AENTDefaultCharacter::UpdateSpikeLocation(float DeltaTime) const
{
	float Alpha = DeltaTime * SpikeLerpSpeed;

	FVector CurrentLocation = bUseSpikeRelativeTransform ? SpikeMesh->GetRelativeLocation() : SpikeMesh->GetComponentLocation();
	FVector TargetLocation = UKismetMathLibrary::VLerp(CurrentLocation, SpikeTargetTransform.GetLocation(), Alpha);

	if (bUseSpikeRelativeTransform)
	{
		FRotator TargetRotator = UKismetMathLibrary::RLerp(SpikeMesh->GetRelativeRotation(), SpikeTargetTransform.GetRotation().Rotator(), Alpha, true);

		SpikeMesh->SetRelativeRotation(TargetRotator);
		SpikeMesh->SetRelativeLocation(TargetLocation);
	}
	else
	{
		SpikeMesh->SetWorldLocation(TargetLocation);
	}
}

#pragma endregion

#pragma region CharacterFunctions

FVector AENTDefaultCharacter::GetBottomLocation() const
{
	return GetPlayerLocation(false);
}

FVector AENTDefaultCharacter::GetTopLocation() const
{
	return GetPlayerLocation(true);
}

FVector AENTDefaultCharacter::GetPlayerLocation(bool TopLocation) const
{
	FVector TargetLocation = GetActorLocation();
	TargetLocation.Z += GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * (TopLocation ? 1 : -1);
	return TargetLocation;
}

bool AENTDefaultCharacter::GetSlopeProperties(float& SlopeAngle, FVector& SlopeNormal) const
{
	FHitResult Hit;
	if (!GroundTrace(Hit))
	{
		return false;
	}

	SlopeNormal = Hit.ImpactNormal;

	float DotResult = FVector::DotProduct(SlopeNormal, FVector::UpVector);

	SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(DotResult));

	return true;
}

void AENTDefaultCharacter::EjectCharacter(const FVector ProjectionVelocity, bool bOverrideCurrentVelocity) const
{
	UENTCharacterFallState* FallState = FindState<UENTCharacterFallState>(StateMachine);
	if (!FallState)
	{
		return;
	}

	FallState->SetProjectionVelocity(ProjectionVelocity, bOverrideCurrentVelocity);
	StateMachine->ChangeState(EENTCharacterStateID::Fall);
}

void AENTDefaultCharacter::StopCharacter() const
{
	if (!StateMachine)
	{
		return;
	}

	StateMachine->ChangeState(EENTCharacterStateID::Stop);
}

bool AENTDefaultCharacter::IsStopped() const
{
	return StateMachine->GetCurrentStateID() == EENTCharacterStateID::Stop;
}

#pragma endregion

#pragma region Saves

#if WITH_EDITOR
void AENTDefaultCharacter::SavePlayer()
{
	SaveGameElement(nullptr);
}
#endif

FENTGameElementData& AENTDefaultCharacter::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	UENTPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UENTPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem)
	{
		return EmptyData;
	}

	if (CurrentWorldSave)
	{
		CurrentWorldSave->PlayerLocation = GetActorLocation();
		CurrentWorldSave->PlayerCameraRotation = GetControlRotation();
		CurrentWorldSave->LastCheckPointName = GetRespawnTree() ? GetRespawnTree().GetName() : "";
	}

	PlayerSaveSubsystem->GetPlayerSave()->CurrentState = static_cast<uint8>(StateMachine->GetCurrentStateID());
	PlayerSaveSubsystem->SaveToSlot(0);

	return EmptyData;
}

void AENTDefaultCharacter::LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave)
{
	UENTPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UENTPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem)
	{
		return;
	}

	if (LoadedWorldSave)
	{
		SetActorLocation(LoadedWorldSave->PlayerLocation);

		if (GetPlayerController())
		{
			APlayerController* PlayerController = Cast<APlayerController>(GetPlayerController());
			if (PlayerController)
			{
				PlayerController->SetControlRotation(LoadedWorldSave->PlayerCameraRotation);
			}
		}
	}

	TObjectPtr<UENTPlayerSave> SaveData = PlayerSaveSubsystem->GetPlayerSave();
	StateMachine->ChangeState(static_cast<EENTCharacterStateID>(SaveData->CurrentState));

	AmberInventory.Empty(SaveData->AmberInventory.Num());
	for (const TTuple<uint8, int>& Element : SaveData->AmberInventory)
	{
		AmberInventory.Add(static_cast<EAmberType>(Element.Key), Element.Value);
	}

	for (const TTuple<EAmberType, int>& Element : AmberInventory)
	{
		OnAmberUpdate.Broadcast(Element.Key, Element.Value);
	}
}

#pragma endregion

#pragma region Respawn

void AENTDefaultCharacter::Respawn()
{
	AENTRespawnTree* RespawnTree = GetRespawnTree();
	FTransform RespawnTransform(FTransform::Identity);

	if (RespawnTree)
	{
		RespawnTransform = RespawnTree->GetRespawnTransform();
	}
	else
	{
		AActor* PlayerStart = UGameplayStatics::GetActorOfClass(this, APlayerStart::StaticClass());
		if (PlayerStart)
		{
			RespawnTransform = PlayerStart->GetActorTransform();
		}

#if WITH_EDITOR
		else
		{
			const FString Message = FString::Printf(TEXT("Missing playerStart in this level"));

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
			FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
		}
#endif
	}

	FRotator CurrentRotation = GetActorRotation();
	FRotator RespawnRotation = RespawnTransform.GetRotation().Rotator();
	RespawnRotation.Pitch = CurrentRotation.Pitch;
	RespawnRotation.Roll = CurrentRotation.Roll;

	RespawnTransform.SetRotation(RespawnRotation.Quaternion());
	RespawnTransform.SetScale3D(FVector::OneVector);

	SetActorTransform(RespawnTransform);
	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	OnRespawn.Broadcast();
}

void AENTDefaultCharacter::OnPlayerDie()
{
	if (!FirstPersonController)
	{
		return;
	}

	HealthComponent->ResetHealth();
}

#pragma endregion

#pragma region Sounds

void AENTDefaultCharacter::ResetFootStepsEvent() const
{
	FootstepsSounds->AkAudioEvent = DefaultFootStepEvent;
}

UENTPropulsionConstraint* AENTDefaultCharacter::AddConstraint()
{
	UActorComponent* Comp = AddComponentByClass(UENTPropulsionConstraint::StaticClass(), false, FTransform::Identity, false);
	if (!Comp)
	{
		return nullptr;
	}

	UENTPropulsionConstraint* Constraint = Cast<UENTPropulsionConstraint>(Comp);
	OnConstraintAdded.Broadcast(Constraint);
	return Constraint;
}

#pragma endregion
