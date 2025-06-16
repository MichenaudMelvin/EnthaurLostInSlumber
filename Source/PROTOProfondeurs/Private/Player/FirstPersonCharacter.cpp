// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/FirstPersonCharacter.h"
#include "AkComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Interface/GroundAction.h"
#include "Camera/CameraComponent.h"
#include "Components/CameraShakeComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InteractableComponent.h"
#include "GameElements/AmberOre.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/InGameUI.h"
#include "Player/FirstPersonController.h"
#include "Player/States/CharacterState.h"
#include "Player/States/CharacterStateMachine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Physics/TracePhysicsSettings.h"
#include "Player/CharacterSettings.h"
#include "Runtime/AIModule/Classes/Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Physics/SurfaceSettings.h"
#include "Player/States/CharacterFallState.h"
#include "Saves/PlayerSave.h"
#include "Saves/PlayerSaveSubsystem.h"
#include "UI/DeathMenuUI.h"

#if WITH_EDITORONLY_DATA
#include "EditorSettings/PlayerEditorSettings.h"
#endif

AFirstPersonCharacter::AFirstPersonCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f));
	CameraComponent->bUsePawnControlRotation = true;

	ShakeManager = CreateDefaultSubobject<UCameraShakeComponent>(TEXT("Shake Manager"));

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

	AmberInventory.Add(EAmberType::NecroseAmber, 0);
	AmberInventory.Add(EAmberType::WeakAmber, 0);

	AmberInventoryMaxCapacity.Add(EAmberType::NecroseAmber, 3);
	AmberInventoryMaxCapacity.Add(EAmberType::WeakAmber, 1);
}

void AFirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpikeRelativeTransform = SpikeMesh->GetRelativeTransform();
	SpikeTargetTransform = SpikeRelativeTransform;
	SpikeParent = SpikeMesh->GetAttachParent();

	if (!GetController())
	{
		return;
	}

	AFirstPersonController* CastedController = Cast<AFirstPersonController>(GetController());
	if (!CastedController)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("PlayerController of %s is %s but should be %s"), *GetClass()->GetName(), *GetController()->GetClass()->GetName(), *AFirstPersonController::StaticClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
		return;
	}

	FirstPersonController = CastedController;

	const UCharacterSettings* CharacterSettings = GetDefault<UCharacterSettings>();
	if (!CharacterSettings)
	{
		return;
	}

	UCameraShakeBase* CameraShake = FirstPersonController->PlayerCameraManager->StartCameraShake(CharacterSettings->ViewBobbingClass, 1.0f, ECameraShakePlaySpace::World);
	if (!CameraShake)
	{
		return;
	}

	UViewBobbing* CastedCameraShake = Cast<UViewBobbing>(CameraShake);
	if (!CastedCameraShake)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("ViewBobbingClass in %s is class of %s but should be %s"), *CharacterSettings->GetClass()->GetName(), *CameraShake->GetClass()->GetName(), *UViewBobbing::StaticClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
		return;
	}

	ViewBobbing = CastedCameraShake;

	DefaultFootStepEvent = FootstepsSounds->AkAudioEvent;

	CreateStates();
	InitStateMachine();

	if (!StartWidgetClass)
	{
		return;
	}

	StartWidget = CreateWidget(FirstPersonController, StartWidgetClass);

#if WITH_EDITORONLY_DATA
	const UPlayerEditorSettings* PlayerEditorSettings = GetDefault<UPlayerEditorSettings>();

	if (!PlayerEditorSettings || !PlayerEditorSettings->bDisplayStartWidget)
	{
		return;
	}
#endif

	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(FirstPersonController, StartWidget);

	StartWidget->AddToViewport(4);
}

void AFirstPersonCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickStateMachine(DeltaSeconds);
	InteractionTrace();
	GroundMovement();
	UpdateSpikeLocation(DeltaSeconds);

	if (CurrentInteractable && GetPlayerController()->GetPlayerInputs().bInputInteractPressed)
	{
		CurrentInteractable->Interact(GetPlayerController(), this);
	}
}

#pragma region StateMachine

void AFirstPersonCharacter::InitStateMachine()
{
	StateMachine = NewObject<UCharacterStateMachine>();

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

void AFirstPersonCharacter::TickStateMachine(float DeltaTime)
{
	if (!StateMachine)
	{
		return;
	}

	StateMachine->TickStateMachine(DeltaTime);
}

void AFirstPersonCharacter::CreateStates()
{
	for (const TTuple<ECharacterStateID, TSubclassOf<UCharacterState>>& State : CharacterStates)
	{
		if (State.Key == ECharacterStateID::None)
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

		UCharacterState* NewState = NewObject<UCharacterState>(this, State.Value);
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
void AFirstPersonCharacter::DisplayStates(bool bDisplay)
{
	bDebugStates = bDisplay;
}
#endif

#pragma endregion

#pragma region Interaction

void AFirstPersonCharacter::InteractionTrace()
{
	const UTracePhysicsSettings* TraceSettings =  GetDefault<UTracePhysicsSettings>();

	if (!TraceSettings)
	{
		return;
	}

	FVector StartLocation = CameraComponent->GetComponentLocation();
	FVector EndLocation = (CameraComponent->GetForwardVector() * InteractionTraceLength) + StartLocation;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, TraceSettings->InteractionTraceChannel, CollisionParams);

	if (!bHit || !HitResult.GetActor())
	{
		RemoveInteraction();
		return;
	}

	UActorComponent* FoundComp = HitResult.GetActor()->GetComponentByClass(UInteractableComponent::StaticClass());
	if (!FoundComp)
	{
		RemoveInteraction();
		return;
	}

	UInteractableComponent* TargetInteractable = Cast<UInteractableComponent>(FoundComp);
	if (!TargetInteractable)
	{
		RemoveInteraction();
		return;
	}

	if (TargetInteractable->CheckComponent(HitResult.GetComponent()))
	{
		SetInteractionUI(true);
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

void AFirstPersonCharacter::RemoveInteraction()
{
	SetInteractionUI(false);
	if(!CurrentInteractable)
	{
		return;
	}

	CurrentInteractable->SelectPrimitive(nullptr);
	CurrentInteractable = nullptr;
}

void AFirstPersonCharacter::SetInteractionUI(const bool bState) const
{
	if (CurrentInteractable != nullptr)
	{
		GetPlayerController()->GetCurrentInGameUI()->SetInteraction(bState);
	}
}

#pragma endregion

#pragma region Ground

void AFirstPersonCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	AboveActor(Hit.GetActor());

	FHitResult HitResult;
	bool bHit = GroundTrace(HitResult);

	if (!bHit || !HitResult.PhysMaterial.IsValid())
	{
		return;
	}

	const USurfaceSettings* SurfaceSettings = GetDefault<USurfaceSettings>();
	if (!SurfaceSettings)
	{
		return;
	}

	const UAkSwitchValue* SurfaceNoise = SurfaceSettings->FindNoise(HitResult.PhysMaterial->SurfaceType);
	if (SurfaceNoise)
	{
		FootstepsSounds->SetSwitch(SurfaceNoise);
	}

	FootstepsSounds->PostAkEvent(LandedEvent);
	ResetFootStepsEvent();
}

bool AFirstPersonCharacter::GroundTrace(FHitResult& HitResult) const
{
	const UTracePhysicsSettings* TraceSettings =  GetDefault<UTracePhysicsSettings>();

	if (!TraceSettings)
	{
		return false;
	}

	FVector StartLocation = GetBottomLocation();
	FVector EndLocation = StartLocation;
	EndLocation.Z -= GroundTraceLength;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bReturnPhysicalMaterial = true;

	// const FCollisionObjectQueryParams ObjectParams = ConfigureCollisionObjectParams(TraceSettings->GroundObjectTypes);

	TArray<AActor*> ActorsToIgnore;

	return UKismetSystemLibrary::LineTraceSingleForObjects(this, StartLocation, EndLocation, TraceSettings->GroundObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	// return GetWorld()->LineTraceSingleByObjectType(HitResult, StartLocation, EndLocation, ObjectParams, CollisionQueryParams);
}

void AFirstPersonCharacter::GroundMovement()
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

void AFirstPersonCharacter::AboveActor(AActor* ActorBellow)
{
	if (!ActorBellow || ActorBellow == GroundActor)
	{
		return;
	}

	GroundActor = ActorBellow;

	if (GroundActor->Implements<UGroundAction>())
	{
		IGroundAction::Execute_OnActorAbove(GroundActor, this);
	}
}

#pragma endregion

#pragma region Amber

void AFirstPersonCharacter::OnEnterWeakZone_Implementation(bool bIsZoneActive)
{
	IWeakZoneInterface::OnEnterWeakZone_Implementation(bIsZoneActive);
}

void AFirstPersonCharacter::OnExitWeakZone_Implementation()
{
	IWeakZoneInterface::OnExitWeakZone_Implementation();
}

void AFirstPersonCharacter::MineAmber(const EAmberType& AmberType, const int Amount)
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

	UPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem)
	{
		return;
	}

	PlayerSaveSubsystem->GetPlayerSave()->AmberInventory = AmberInventory;
}

void AFirstPersonCharacter::UseAmber(const EAmberType& AmberType, const int Amount)
{
	MineAmber(AmberType, -Amount);
}

bool AFirstPersonCharacter::IsAmberTypeFilled(const EAmberType& AmberType) const
{
	const int* Count = AmberInventory.Find(AmberType);
	const int* MaxCapacity = AmberInventoryMaxCapacity.Find(AmberType);

	if (!Count || !MaxCapacity)
	{
		return false;
	}

	return *Count == *MaxCapacity;
}

bool AFirstPersonCharacter::HasRequiredQuantity(const EAmberType& AmberType, const int Quantity) const
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

void AFirstPersonCharacter::PlantSpike(const FVector& TargetLocation)
{
	SpikeMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	bUseSpikeRelativeTransform = false;
	SpikeTargetTransform = FTransform(TargetLocation);
}

void AFirstPersonCharacter::ReGrabSpike()
{
	SpikeMesh->AttachToComponent(SpikeParent, FAttachmentTransformRules::KeepWorldTransform);
	bUseSpikeRelativeTransform = true;
	SpikeTargetTransform = SpikeRelativeTransform;
}

void AFirstPersonCharacter::UpdateSpikeOffset(float Alpha) const
{
	FVector DefaultLocation = SpikeRelativeTransform.GetLocation();
	FVector OffsetLocation = DefaultLocation - (FVector::ForwardVector * SpikeChargingOffset);
	FVector TargetLocation = UKismetMathLibrary::VLerp(DefaultLocation, OffsetLocation, Alpha);

	SpikeMesh->SetRelativeLocation(TargetLocation);
}

void AFirstPersonCharacter::UpdateSpikeLocation(float DeltaTime) const
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

FVector AFirstPersonCharacter::GetBottomLocation() const
{
	return GetPlayerLocation(false);
}

FVector AFirstPersonCharacter::GetTopLocation() const
{
	return GetPlayerLocation(true);
}

FVector AFirstPersonCharacter::GetPlayerLocation(bool TopLocation) const
{
	FVector TargetLocation = GetActorLocation();
	TargetLocation.Z += GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * (TopLocation ? 1 : -1);
	return TargetLocation;
}

bool AFirstPersonCharacter::GetSlopeProperties(float& SlopeAngle, FVector& SlopeNormal) const
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

void AFirstPersonCharacter::EjectCharacter(const FVector ProjectionVelocity, bool bOverrideCurrentVelocity) const
{
	UCharacterFallState* FallState = FindState<UCharacterFallState>(StateMachine);
	if (!FallState)
	{
		return;
	}

	FallState->SetProjectionVelocity(ProjectionVelocity, bOverrideCurrentVelocity);
	StateMachine->ChangeState(ECharacterStateID::Fall);
}

void AFirstPersonCharacter::StopCharacter() const
{
	if (!StateMachine)
	{
		return;
	}

	StateMachine->ChangeState(ECharacterStateID::Stop);
}

bool AFirstPersonCharacter::IsStopped() const
{
	return StateMachine->GetCurrentStateID() == ECharacterStateID::Stop;
}

#pragma endregion

#pragma region Saves

void AFirstPersonCharacter::SavePlayerData() const
{
	UPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem)
	{
		return;
	}

	PlayerSaveSubsystem->GetPlayerSave()->PlayerLocation = GetActorLocation();
	PlayerSaveSubsystem->GetPlayerSave()->PlayerCameraRotation = GetControlRotation();
	PlayerSaveSubsystem->GetPlayerSave()->CurrentState = StateMachine->GetCurrentStateID();
	PlayerSaveSubsystem->SaveToSlot(0);
}

void AFirstPersonCharacter::LoadPlayerData()
{
	UPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem)
	{
		return;
	}

	TObjectPtr<UPlayerSave> SaveData = PlayerSaveSubsystem->GetPlayerSave();
	SetActorLocation(SaveData->PlayerLocation);

	if (GetPlayerController())
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetPlayerController());
		if (PlayerController)
		{
			PlayerController->SetControlRotation(SaveData->PlayerCameraRotation);
		}
	}

	StateMachine->ChangeState(SaveData->CurrentState);
	AmberInventory = SaveData->AmberInventory;

	for (TTuple<EAmberType, int> Element : AmberInventory)
	{
		OnAmberUpdate.Broadcast(Element.Key, Element.Value);
	}
}

#pragma endregion

#pragma region Respawn

void AFirstPersonCharacter::Respawn(const FTransform& RespawnTransform)
{
	//SetActorTransform(RespawnTransform);
	SetActorLocation(RespawnTransform.GetLocation());
	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	OnRespawn.Broadcast();
}

void AFirstPersonCharacter::KillPlayer()
{
	if (!FirstPersonController || !FirstPersonController->GetDeathMenuUI())
	{
		return;
	}

	FirstPersonController->SetPause(true);
	FirstPersonController->GetDeathMenuUI()->AddToViewport();
}

#pragma endregion

#pragma region Sounds

void AFirstPersonCharacter::ResetFootStepsEvent() const
{
	FootstepsSounds->AkAudioEvent = DefaultFootStepEvent;
}

#pragma endregion