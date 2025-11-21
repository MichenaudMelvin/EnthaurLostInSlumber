// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/ENTCharacterFallState.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/ENTDefaultCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"

UENTCharacterFallState::UENTCharacterFallState()
{
	StateID = EENTCharacterStateID::Fall;
}

void UENTCharacterFallState::StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	if (PreviousStateID != EENTCharacterStateID::Jump && PreviousStateID != EENTCharacterStateID::Fall && PreviousStateID != EENTCharacterStateID::Stop)
	{
		bCanDoCoyoteTime = true;
		CoyoteTime = 0.0f;
	}
	else
	{
		bCanDoCoyoteTime = false;
	}

	bHasPressedInteraction = false;
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	Character->GetCharacterMovement()->GravityScale = GravityScale;
	Character->GetCharacterMovement()->AirControl = AirControl;
	Character->GetCharacterMovement()->BrakingDecelerationFalling = FallingDeceleration;
	LaunchCharacter();
}

void UENTCharacterFallState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	if(bCanDoCoyoteTime)
	{
		CoyoteTime += DeltaTime;
		if(CoyoteTime >= CoyoteTimeMaxDuration)
		{
			bCanDoCoyoteTime = false;

#if WITH_EDITORONLY_DATA
			if(bDebugState)
			{
				FVector CapsuleLocation = Character->GetCapsuleComponent()->GetComponentLocation();
				FRotator CapsuleRotation = Character->GetCapsuleComponent()->GetComponentRotation();
				float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
				UKismetSystemLibrary::DrawDebugCapsule(Character, CapsuleLocation, CapsuleHalfHeight, CapsuleRadius, CapsuleRotation, FLinearColor::Red, 5.0f);
			}
#endif
		}

		if (GetInputs().bInputJump)
		{
			StateMachine->ChangeState(EENTCharacterStateID::Jump);
		}
	}

	if(Character->GetCharacterMovement()->IsFalling())
	{
		return;
	}

	// emit noise on landing
	EmitNoise();

	SetProjectionVelocity(FVector::ZeroVector, false);
	StateMachine->ChangeState(EENTCharacterStateID::Idle);
}

void UENTCharacterFallState::SetProjectionVelocity(const FVector& Velocity, bool bOverrideVelocity)
{
	ProjectionVelocity = Velocity;
	bOverrideCurrentVelocity = bOverrideVelocity;
}

void UENTCharacterFallState::LaunchCharacter() const
{
	FVector TargetVelocity = ProjectionVelocity;
	if (!bOverrideCurrentVelocity)
	{
		TargetVelocity += Character->GetCharacterMovement()->Velocity;
	}

	Character->GetCharacterMovement()->Velocity = TargetVelocity;
}
