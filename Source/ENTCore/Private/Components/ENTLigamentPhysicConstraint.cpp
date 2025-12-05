// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ENTLigamentPhysicConstraint.h"

#include "AkGameplayStatics.h"
#include "GameElements/ENTNerve.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"


void UENTLigamentPhysicConstraint::Init(AENTNerve* vLinkedNerve, ACharacter* vPlayerCharacter)
{
	Super::Init(vLinkedNerve, vPlayerCharacter);

	if (!bShouldChildInit) return;

	UpdateDefaultMaxSpeed(PlayerStateMachine->GetCurrentState(), PlayerStateMachine->GetCurrentStateID());
	PlayerStateMachine->OnChangeState.AddDynamic(this, &UENTPhysicConstraint::UpdateDefaultMaxSpeed);

	PlayerStateMachine->LockCameraMovements(true);
}

void UENTLigamentPhysicConstraint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (PlayerStateMachine->OnChangeState.IsAlreadyBound(this, &UENTPhysicConstraint::UpdateDefaultMaxSpeed))
	{
		PlayerStateMachine->OnChangeState.RemoveDynamic(this, &UENTPhysicConstraint::UpdateDefaultMaxSpeed);
	}
}

void UENTLigamentPhysicConstraint::ReleasePlayer(const bool DetachFromPlayer)
{
	if (PlayerCharacter && PlayerController)
	{
		bIsPropulsionActive = false;
		OnPropulsionStateChanged.Broadcast(bIsPropulsionActive);
		PlayerStateMachine->LockCameraMovements(false);
	}
	
	LinkedNerve->GetDynamicCableStretchedMaterial()->SetScalarParameterValue(FName("VibrationStrength"), 0.f);
	Super::ReleasePlayer(DetachFromPlayer);
}

void UENTLigamentPhysicConstraint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bShouldChildTick) return;

	float Distance = LinkedNerve->GetCableLength();
	float Lerp = UKismetMathLibrary::NormalizeToRange(Distance, 0.0f, LinkedNerve->GetCableMaxExtension());

	Lerp = FMath::Clamp(Lerp, 0.0f, 1.0f);
	Lerp = FMath::Sin((Lerp * PI) / 2.0f);

	if (IsMovingTowardsPosition(LinkedNerve->GetLastCableLocation(), 0.2f))
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed * (1.0f + Lerp);
	}
	else
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed * (1.0f - Lerp);
	}

	const FVector CableDirection = UKismetMathLibrary::GetDirectionUnitVector(
		PlayerCharacter->GetActorLocation(), LinkedNerve->GetStartCableLocation());

	//Jump Direction Is Set By Cable Direction + An AngularBuff (Positive Or Negative) Set By Designers
	FVector CableDirectionGround = CableDirection;
	CableDirectionGround.Z = 0.f;
	CableDirectionGround = CableDirectionGround.GetSafeNormal();

	const FVector RotationAxis = FVector::CrossProduct(CableDirectionGround, CableDirection).GetSafeNormal();

	const float AngleBuff = LinkedNerve->GetEjectionAngleBuff();
	const FQuat RotationQuat = FQuat(RotationAxis, FMath::DegreesToRadians(AngleBuff));

	const FVector JumpDirection = RotationQuat.RotateVector(CableDirection).GetSafeNormal();
	
	FVector JumpHorizontalDirection = JumpDirection;
	JumpHorizontalDirection.Z = 0.f;
	JumpHorizontalDirection = JumpHorizontalDirection.GetSafeNormal();

	FVector CameraDirection = (JumpHorizontalDirection + JumpDirection).GetSafeNormal();

	//on dirige la caméra vers le point d'attache si c'est un ligament
	const FRotator TargetCameraRotation = CameraDirection.Rotation();

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
			UAkGameplayStatics::PostEvent(LinkedNerve->LigamentTenseEvent, PlayerCharacter, 0, FOnAkPostEventCallback());
		}

		const float Vibration = Lerp * (LinkedNerve -> GetMaxVibrationStrength());
		LinkedNerve->GetDynamicCableStretchedMaterial()->SetScalarParameterValue(FName("VibrationStrength"), Vibration);
		
		if (!PlayerController->GetPlayerInputs().bInputInteractTrigger && !bIsAlreadyPropelled)
		{
			bIsAlreadyPropelled = true;
			
			const float Force = FMath::Lerp(LinkedNerve->GetPropulsionForceRange().GetLowerBoundValue(), LinkedNerve->GetPropulsionForceRange().GetUpperBoundValue(), Lerp);

			PlayerCharacter->EjectCharacter(JumpDirection * Force, false);
			UAkGameplayStatics::PostEvent(LinkedNerve->StopLigamentTenseEvent, PlayerCharacter, 0, FOnAkPostEventCallback());
			UAkGameplayStatics::PostEvent(LinkedNerve->LigamentReleaseEvent, PlayerCharacter, 0, FOnAkPostEventCallback());
			ReleasePlayer(true);
		}
	} else if (Distance < LinkedNerve->GetDistanceNeededToPropulsion())
	{
		if (bIsPropulsionActive)
		{
			bIsPropulsionActive = false;
			UAkGameplayStatics::PostEvent(LinkedNerve->StopLigamentTenseEvent, PlayerCharacter, 0, FOnAkPostEventCallback());
			OnPropulsionStateChanged.Broadcast(bIsPropulsionActive);
			LinkedNerve->GetDynamicCableStretchedMaterial()->SetScalarParameterValue(FName("VibrationStrength"), 0.f);
		}

		else if (!PlayerController->GetPlayerInputs().bInputInteractTrigger)
		{
			ReleasePlayer(true);
		}
	}
}

bool UENTLigamentPhysicConstraint::IsMovingTowardsPosition(const FVector& TargetPosition, float AcceptanceThreshold) const
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