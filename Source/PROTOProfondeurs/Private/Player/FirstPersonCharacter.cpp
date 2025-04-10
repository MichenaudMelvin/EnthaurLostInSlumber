// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/FirstPersonCharacter.h"
#include "Interface/GroundAction.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InteractableComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/InGameUI.h"
#include "Player/FirstPersonController.h"
#include "Player/States/CharacterState.h"
#include "Player/States/CharacterStateMachine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "KismetTraceUtils.h"
#include "GameFramework/GameStateBase.h"
#include "UI/DeathMenuUI.h"
#include "Kismet/GameplayStatics.h"
#include "Physics/TracePhysicsSettings.h"
#include "Player/CharacterSettings.h"
#include "PRFUI/Public/TestMVVM/TestViewModel.h"
#include "Runtime/AIModule/Classes/Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Player/States/CharacterFallState.h"

AFirstPersonCharacter::AFirstPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);

	// Create a CameraComponent
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f)); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	CharacterMesh->SetOnlyOwnerSee(true);
	CharacterMesh->SetupAttachment(CameraComponent);
	CharacterMesh->bCastDynamicShadow = false;
	CharacterMesh->CastShadow = false;
	CharacterMesh->SetRelativeLocation(FVector(-30.0f, 0.0f, -150.0f));

	HearingStimuli = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("Hearing");
	HearingStimuli->bAutoRegister = true;
	HearingStimuli->RegisterForSense(UAISense_Hearing::StaticClass());
}

void AFirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetRespawnPosition(this->GetActorLocation());
	
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

	CreateStates();
	InitStateMachine();

	ViewModel = NewObject<UTestViewModel>();
	ensure(ViewModel);
}

void AFirstPersonCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickStateMachine(DeltaSeconds);
	InteractionTrace();
	GroundMovement();
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
		SetInteractionUI(false);
		CurrentInteractable = nullptr;
		return;
	}

	UActorComponent* FoundComp = HitResult.GetActor()->GetComponentByClass(UInteractableComponent::StaticClass());
	if (!FoundComp)
	{
		SetInteractionUI(false);
		CurrentInteractable = nullptr;
		return;
	}

	UInteractableComponent* TargetInteractable = Cast<UInteractableComponent>(FoundComp);
	if (!TargetInteractable)
	{
		SetInteractionUI(false);
		CurrentInteractable = nullptr;
		return;
	}

	if (TargetInteractable->CheckComponent(HitResult.GetComponent()))
	{
		SetInteractionUI(true);
		CurrentInteractable = TargetInteractable;

#if WITH_EDITORONLY_DATA
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("Interaction: %s"), *CurrentInteractable->GetInteractionName().ToString()));
#endif
	}
	else
	{
		SetInteractionUI(false);
		CurrentInteractable = nullptr;
	}
}

#pragma endregion

#pragma region Ground

void AFirstPersonCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	AboveActor(Hit.GetActor());
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

	bCanTakeAmber = bIsZoneActive;
}

void AFirstPersonCharacter::OnExitWeakZone_Implementation()
{
	IWeakZoneInterface::OnExitWeakZone_Implementation();

	bCanTakeAmber = false;
}

#pragma endregion

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

void AFirstPersonCharacter::EjectCharacter(const FVector ProjectionVelocity) const
{
	UCharacterFallState* FallState = FindState<UCharacterFallState>(StateMachine);
	if (!FallState)
	{
		return;
	}

	FallState->SetProjectionVelocity(ProjectionVelocity);
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

void AFirstPersonCharacter::SetInteractionUI(const bool bState) const
{
	if (CurrentInteractable != nullptr)
		GetPlayerController()->GetCurrentInGameUI()->SetInteraction(bState);
}
