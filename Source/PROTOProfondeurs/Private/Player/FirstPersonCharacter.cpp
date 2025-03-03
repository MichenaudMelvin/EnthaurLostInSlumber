// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/FirstPersonCharacter.h"
#include "GroundAction.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InteractableComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/FirstPersonController.h"
#include "Player/States/CharacterState.h"
#include "Player/States/CharacterStateMachine.h"

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
}

void AFirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetController() == nullptr)
	{
		return;
	}

	AFirstPersonController* CastedController = Cast<AFirstPersonController>(GetController());
	if (CastedController == nullptr)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("PlayerController of %s is %s but should be %s"), *GetClass()->GetName(), *GetController()->GetClass()->GetName(), *AFirstPersonController::StaticClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
		return;
	}

	FirstPersonController = CastedController;

	CreateStates();
	InitStateMachine();
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

	if (StateMachine == nullptr)
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
	if (StateMachine == nullptr)
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

		if (State.Value == nullptr)
		{
#if WITH_EDITOR
			const FString Message = FString::Printf(TEXT("Cannot create state because stateClass is nullptr"));

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
			FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
			continue;
		}

		UCharacterState* NewState = NewObject<UCharacterState>(this, State.Value);
		if (NewState == nullptr)
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
	FVector StartLocation = CameraComponent->GetComponentLocation();
	FVector EndLocation = (CameraComponent->GetForwardVector() * InteractionTraceLength) + StartLocation;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);

	if (!bHit)
	{
		CurrentInteractable = nullptr;
		return;
	}

	if (HitResult.GetActor() == nullptr)
	{
		CurrentInteractable = nullptr;
		return;
	}

	UActorComponent* FoundComp = HitResult.GetActor()->GetComponentByClass(UInteractableComponent::StaticClass());
	if (FoundComp == nullptr)
	{
		CurrentInteractable = nullptr;
		return;
	}

	UInteractableComponent* TargetInteractable = Cast<UInteractableComponent>(FoundComp);
	if (TargetInteractable == nullptr)
	{
		CurrentInteractable = nullptr;
		return;
	}

	if (TargetInteractable->CheckComponent(HitResult.GetComponent()))
	{
		CurrentInteractable = TargetInteractable;

#if WITH_EDITORONLY_DATA
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("Interaction: %s"), *CurrentInteractable->GetInteractionName().ToString()));
#endif
	}
	else
	{
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
	FVector StartLocation = GetBottomLocation();
	FVector EndLocation = StartLocation;
	EndLocation.Z -= GroundTraceLength;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bReturnPhysicalMaterial = true;

	return GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionQueryParams);
}

void AFirstPersonCharacter::GroundMovement()
{
	FHitResult Hit;
	if (!GetCharacterMovement()->IsFalling() && GroundTrace(Hit))
	{
		AboveActor(Hit.GetActor());
	}

	if (GetCharacterMovement()->IsFalling() && GroundActor != nullptr)
	{
		GroundActor = nullptr;
	}
}

void AFirstPersonCharacter::AboveActor(AActor* ActorBellow)
{
	if (ActorBellow == nullptr)
	{
		return;
	}

	if (ActorBellow == GroundActor)
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

FVector AFirstPersonCharacter::GetBottomLocation() const
{
	FVector TargetLocation = GetActorLocation();
	TargetLocation.Z -= GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	return TargetLocation;
}

FVector AFirstPersonCharacter::GetTopLocation() const
{
	FVector TargetLocation = GetActorLocation();
	TargetLocation.Z += GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
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
