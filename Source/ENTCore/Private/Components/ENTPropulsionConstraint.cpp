// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ENTPropulsionConstraint.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameElements/ENTNerve.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Config/ENTCoreConfig.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"
#include "Player/States/ENTCharacterMoveState.h"

UENTPropulsionConstraint::UENTPropulsionConstraint()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UENTPropulsionConstraint::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<AENTDefaultPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
}

void UENTPropulsionConstraint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!PlayerCharacter)
	{
		return;
	}

	UENTCharacterStateMachine* StateMachine = PlayerCharacter->GetStateMachine();
	if (!StateMachine)
	{
		return;
	}

	if (StateMachine->OnChangeState.IsAlreadyBound(this, &UENTPropulsionConstraint::UpdateDefaultMaxSpeed))
	{
		StateMachine->OnChangeState.RemoveDynamic(this, &UENTPropulsionConstraint::UpdateDefaultMaxSpeed);
	}
}

void UENTPropulsionConstraint::UpdateDefaultMaxSpeed(UENTCharacterState* State, const EENTCharacterStateID& StateID)
{
	if (!State)
	{
		return;
	}

	UENTCharacterMoveState* MoveState = Cast<UENTCharacterMoveState>(State);
	if (!MoveState)
	{
		return;
	}

	DefaultMaxSpeed = MoveState->GetMoveSpeed();
}

void UENTPropulsionConstraint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PlayerController->GetPlayerInputs().bInputInteractTrigger)
	{
		bHasReleasedInteraction = true;
	}

	if (!LinkedNerve)
	{
		return;
	}

	float Distance = LinkedNerve->GetCableLength();
	float Lerp = UKismetMathLibrary::NormalizeToRange(Distance, 0.0f, LinkedNerve->GetCableMaxExtension());

	Lerp = FMath::Clamp(Lerp, 0.0f, 1.0f);
	Lerp = FMath::Sin((Lerp * PI) / 2.0f);

	if (IsMovingTowardsPosition(LinkedNerve->GetLastCableLocation(), 0.2f))
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed * (1.0f + Lerp);
	} else
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed * (1.0f - Lerp);
	}

	if (!bHasReleasedInteraction)
	{
		return;
	}
	
	//Tourner la cam vers le point d'ancrage
	const FVector CableDirection = UKismetMathLibrary::GetDirectionUnitVector(PlayerCharacter->GetActorLocation(), LinkedNerve->GetStartCableLocation());
	/*
	if (!LinkedNerve->IsLigament())
	{
		const FRotator TargetRotation = CableDirection.Rotation();

		AENTDefaultPlayerController* PC = PlayerCharacter->GetPlayerController();
		if (PC)
		{
			const FRotator CurrentRotation = PC->GetControlRotation();
			constexpr float InterpSpeed = 5.0f;
			const FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);
			PC->SetControlRotation(SmoothRotation);
		}
	}
	*/

	//Jump Direction Is Set By Cable Direction + An AngularBuff (Positive Or Negative) Set By Designers
	FVector CableDirectionGround = CableDirection;
	CableDirectionGround.Z = 0.f;
	CableDirectionGround = CableDirectionGround.GetSafeNormal();
			
	const FVector RotationAxis = FVector::CrossProduct(CableDirectionGround, CableDirection).GetSafeNormal();
			
	const float AngleBuff = LinkedNerve->GetEjectionAngleBuff();
	const FQuat RotationQuat = FQuat(RotationAxis, FMath::DegreesToRadians(AngleBuff));
			
	const FVector JumpDirection = RotationQuat.RotateVector(CableDirection).GetSafeNormal();
	
	const FRotator TargetCameraRotation = JumpDirection.Rotation();

	AENTDefaultPlayerController* PC = PlayerCharacter->GetPlayerController();
	if (PC)
	{
		const FRotator CurrentRotation = PC->GetControlRotation();
		constexpr float InterpSpeed = 5.0f;
		const FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, TargetCameraRotation, DeltaTime, InterpSpeed);
		PC->SetControlRotation(SmoothRotation);
	}

	if (Distance >= LinkedNerve->GetDistanceNeededToPropulsion())
	{
		if (!bIsPropulsionActive)
		{
			bIsPropulsionActive = true;
			OnPropulsionStateChanged.Broadcast(bIsPropulsionActive);
		}

		const float Vibration = Lerp * (LinkedNerve -> GetMaxVibrationStrength());
		LinkedNerve->GetDynamicCableStretchedMaterial()->SetScalarParameterValue(FName("VibrationStrength"), Vibration);
		
		if (PlayerController->GetPlayerInputs().bInputInteractPressed && !bIsAlreadyPropelled)
		{
			bIsAlreadyPropelled = true;
			
			const float Force = FMath::Lerp(LinkedNerve->GetPropulsionForceRange().GetLowerBoundValue(), LinkedNerve->GetPropulsionForceRange().GetUpperBoundValue(), Lerp);

			PlayerCharacter->EjectCharacter(JumpDirection * Force, false);

			ReleasePlayer(true);
		}
	} else if (Distance < LinkedNerve->GetDistanceNeededToPropulsion())
	{
		if (bIsPropulsionActive)
		{
			bIsPropulsionActive = false;
			OnPropulsionStateChanged.Broadcast(bIsPropulsionActive);
			LinkedNerve->GetDynamicCableStretchedMaterial()->SetScalarParameterValue(FName("VibrationStrength"), 0.f);
		}

		else if (PlayerController->GetPlayerInputs().bInputInteractPressed)
		{
			ReleasePlayer(true);
		}
	}
}

void UENTPropulsionConstraint::Init(AENTNerve* vLinkedNerve, ACharacter* vPlayerCharacter)
{
	LinkedNerve = vLinkedNerve;
	AENTDefaultCharacter* CastCharacter = Cast<AENTDefaultCharacter>(vPlayerCharacter);
	if (!CastCharacter)
	{
		return;
	}

	PlayerCharacter = CastCharacter;
	UENTCharacterStateMachine* StateMachine = PlayerCharacter->GetStateMachine();
	if (!StateMachine)
	{
		return;
	}

	//if (!LinkedNerve->IsLigament()) StateMachine->LockCameraMovements(true);
	StateMachine->LockCameraMovements(true);
	
	UpdateDefaultMaxSpeed(StateMachine->GetCurrentState(), StateMachine->GetCurrentStateID());
	StateMachine->OnChangeState.AddDynamic(this, &UENTPropulsionConstraint::UpdateDefaultMaxSpeed);
}

void UENTPropulsionConstraint::ReleasePlayer(const bool DetachFromPlayer)
{
	if (PlayerCharacter && PlayerController)
	{
		bIsPropulsionActive = false;
		OnPropulsionStateChanged.Broadcast(bIsPropulsionActive);
		PlayerCharacter->GetStateMachine()->LockCameraMovements(false);
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed;
	}

	if (DetachFromPlayer && LinkedNerve)
	{
		LinkedNerve->DetachNerveBall(false);
	}

	LinkedNerve->GetDynamicCableStretchedMaterial()->SetScalarParameterValue(FName("VibrationStrength"), 0.f);
	
	DestroyComponent();
}

bool UENTPropulsionConstraint::IsMovingTowardsPosition(const FVector& TargetPosition, float AcceptanceThreshold) const
{
	const UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement();
	if (!MovementComponent)
	{
		return false;
	}

	const FVector Velocity = MovementComponent->Velocity;
	if (Velocity.IsNearlyZero())
	{
		return false;
	}

	const FVector CurrentLocation = PlayerCharacter->GetActorLocation();
	const FVector ToTargetDirection = (TargetPosition - CurrentLocation).GetSafeNormal();
	const FVector MovementDirection = Velocity.GetSafeNormal();

	const float DotProduct = FVector::DotProduct(MovementDirection, ToTargetDirection);
	return DotProduct >= AcceptanceThreshold;
}
