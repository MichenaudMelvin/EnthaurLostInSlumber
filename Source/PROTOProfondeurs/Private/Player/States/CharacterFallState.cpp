// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterFallState.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/FirstPersonCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/FirstPersonController.h"
#include "Player/States/CharacterStateMachine.h"

UCharacterFallState::UCharacterFallState()
{
	StateID = ECharacterStateID::Fall;
}

void UCharacterFallState::StateEnter_Implementation(const ECharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	if (PreviousStateID != ECharacterStateID::Jump && PreviousStateID != ECharacterStateID::Fall && PreviousStateID != ECharacterStateID::Stop)
	{
		bCanDoCoyoteTime = true;
		CoyoteTime = 0.0f;
	}
	else
	{
		bCanDoCoyoteTime = false;
	}

	SpikeBrakePressedDuration = 0.0f;
	SpikeBrakeTimer = 0.0f;
	bHasPressedInteraction = false;
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	Character->GetCharacterMovement()->GravityScale = GravityScale;
	Character->GetCharacterMovement()->AirControl = AirControl;
	Character->GetCharacterMovement()->BrakingDecelerationFalling = FallingDeceleration;
	LaunchCharacter();
}

void UCharacterFallState::StateTick_Implementation(float DeltaTime)
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
			StateMachine->ChangeState(ECharacterStateID::Jump);
		}
	}

	SpikeBrakeTimer += DeltaTime;

	if (Controller->GetPlayerInputs().bInputInteractTrigger && SpikeBrakeTimer >= SpikeBrakeDelay)
	{
		SpikeBrakePressedDuration += DeltaTime;

		FVector StartLocation = Character->GetCamera()->GetComponentLocation();
		FVector EndLocation = (Character->GetCamera()->GetForwardVector() * SpikeBrakeTraceLength) + StartLocation;

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(Character);

		FHitResult Hit;
		bool bHit = UKismetSystemLibrary::LineTraceSingle(Character, StartLocation, EndLocation, SpikeBrakeTraceTypeQuery, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, false);

		if (bHit)
		{
			Character->PlantSpike(Hit.Location);
			StateMachine->ChangeState(ECharacterStateID::Stop);
			return;
		}
	}
	else
	{
		SpikeBrakePressedDuration -= DeltaTime;
	}

	SpikeBrakePressedDuration = FMath::Clamp(SpikeBrakePressedDuration, 0.0f, SpikeBrakeMaxPressedDuration);
	float Alpha = UKismetMathLibrary::NormalizeToRange(SpikeBrakePressedDuration, 0.0f, SpikeBrakeMaxPressedDuration);
	Character->UpdateSpikeOffset(Alpha);

	if(Character->GetCharacterMovement()->IsFalling())
	{
		return;
	}

	// emit noise on landing
	EmitNoise();

	SetProjectionVelocity(FVector::ZeroVector, false);
	StateMachine->ChangeState(ECharacterStateID::Idle);
}

void UCharacterFallState::SetProjectionVelocity(const FVector& Velocity, bool bOverrideVelocity)
{
	ProjectionVelocity = Velocity;
	bOverrideCurrentVelocity = bOverrideVelocity;
}

void UCharacterFallState::LaunchCharacter() const
{
	FVector TargetVelocity = ProjectionVelocity;
	if (!bOverrideCurrentVelocity)
	{
		TargetVelocity += Character->GetCharacterMovement()->Velocity;
	}

	Character->GetCharacterMovement()->Velocity = TargetVelocity;
}
