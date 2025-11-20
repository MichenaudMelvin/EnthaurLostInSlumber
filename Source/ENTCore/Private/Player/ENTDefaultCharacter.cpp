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
#include "Components/ENTLigamentPhysicConstraint.h"
#include "Components/ENTNervePhysicConstraint.h"
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
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Config/ENTCoreConfig.h"
#include "GameElements/ENTWeakZone.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Player/States/ENTCharacterFallState.h"
#include "Saves/ENTPlayerSave.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTWorldSave.h"
#include "Subsystems/ENTPlayerSaveSubsystem.h"

class UEnhancedInputLocalPlayerSubsystem;

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

	FootstepsSounds = CreateDefaultSubobject<UAkComponent>(TEXT("FootstepsSounds"));
	FootstepsSounds->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<UENTHealthComponent>("Health");
}

void AENTDefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

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

	UCameraShakeBase* CameraShake = FirstPersonController->PlayerCameraManager->StartCameraShake(CoreConfig->ViewBobbingClass, 1.0f, ECameraShakePlaySpace::CameraLocal);
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

void AENTDefaultCharacter::OnEnterWeakZone_Implementation(bool bIsZoneActive)
{
	IENTWeakZoneInterface::OnEnterWeakZone_Implementation(bIsZoneActive);
}

void AENTDefaultCharacter::OnExitWeakZone_Implementation()
{
	IENTWeakZoneInterface::OnExitWeakZone_Implementation();
}

#pragma region Amber

void AENTDefaultCharacter::MineAmber()
{
	if (bHasAmber)
	{
		return;
	}

	bHasAmber = true;
	OnAmberUpdate.Broadcast(bHasAmber);

	UENTPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UENTPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem || !PlayerSaveSubsystem->GetPlayerSave())
	{
		return;
	}

	PlayerSaveSubsystem->GetPlayerSave()->bHasAmber = bHasAmber;
}

void AENTDefaultCharacter::UseAmber()
{
	if (!bHasAmber)
	{
		return;
	}

	bHasAmber = false;
	OnAmberUpdate.Broadcast(bHasAmber);

	UENTPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UENTPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem || !PlayerSaveSubsystem->GetPlayerSave())
	{
		return;
	}

	PlayerSaveSubsystem->GetPlayerSave()->bHasAmber = bHasAmber;
}

#if WITH_EDITOR
void AENTDefaultCharacter::IgnoreWeakZone(bool bIgnore) const
{
	TArray<AActor*> WeakZones;
	UGameplayStatics::GetAllActorsOfClass(this, AENTWeakZone::StaticClass(), WeakZones);

	for (AActor* Actor : WeakZones)
	{
		if (!Actor)
		{
			continue;
		}

		AENTWeakZone* WeakZone = Cast<AENTWeakZone>(Actor);
		if (WeakZone)
		{
			WeakZone->ActivateZone(!bIgnore);
		}
	}
}
#endif

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

#if WITH_EDITOR
void AENTDefaultCharacter::EjectCharacterForward(float Force) const
{
	EjectCharacter(CameraComponent->GetForwardVector() * Force, true);
}
#endif

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

	if (StateMachine)
	{
		PlayerSaveSubsystem->GetPlayerSave()->CurrentState = static_cast<uint8>(StateMachine->GetCurrentStateID());
	}

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

	bHasAmber = SaveData->bHasAmber;
	OnAmberUpdate.Broadcast(bHasAmber);
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

void AENTDefaultCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	// Does not call the parent function because it destroys the actor
	// Super::FellOutOfWorld(dmgType);

	HealthComponent->TakeMaxDamages();
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

UENTPhysicConstraint* AENTDefaultCharacter::AddConstraint(bool bIsLigament)
{
	// Choix de la classe à instancier
	TSubclassOf<UENTPhysicConstraint> ConstraintClass = bIsLigament
		? UENTLigamentPhysicConstraint::StaticClass()
		: UENTNervePhysicConstraint::StaticClass();

	// Création du composant
	UActorComponent* Comp = AddComponentByClass(ConstraintClass, false, FTransform::Identity, false);
	if (!Comp)
	{
		return nullptr;
	}

	UENTPhysicConstraint* Constraint = Cast<UENTPhysicConstraint>(Comp);
	OnConstraintAdded.Broadcast(Constraint);
	return Constraint;
}

#pragma endregion
