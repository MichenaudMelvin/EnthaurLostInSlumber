// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterMoveState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/CharacterSettings.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Player/Camera/ViewBobbing.h"
#include "Saves/SettingsSave.h"

UCharacterMoveState::UCharacterMoveState()
{
	bCheckGround = true;
	bDoesMakeNoise = true;
}

void UCharacterMoveState::StateEnter_Implementation(const ECharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	LockMovement(false);

	Character->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	Character->GetCharacterMovement()->MaxAcceleration = MoveAcceleration;
}

void UCharacterMoveState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	ApplyMovement();
}

void UCharacterMoveState::ApplyMovement()
{
	if (bMovementLocked)
	{
		return;
	}

	Character->AddMovementInput(Character->GetActorForwardVector(), GetInputs().InputMove.Y);
	Character->AddMovementInput(Character->GetActorRightVector(), GetInputs().InputMove.X);

	EmitNoise();

	if(Character->GetViewBobbingObject() && GetSettings() && GetSettings()->bViewBobbing)
	{
		float SpeedRange = UKismetMathLibrary::NormalizeToRange(Character->GetCharacterMovement()->MaxWalkSpeed, 0.0f, MoveSpeed);
		Character->GetViewBobbingObject()->ShakeScale = SpeedRange;
	}
}

void UCharacterMoveState::UpdateCameraSteering(float DeltaTime)
{
	if (FMath::Abs(GetInputs().InputMove.X) < GetDefault<UCharacterSettings>()->MoveInputThreshold)
	{
		TargetSteering = 0.0f;
	}
	else
	{
		TargetSteering = MaxSteering * (GetInputs().InputMove.X > 0.0f ? 1.0f : -1.0f);
	}

	Super::UpdateCameraSteering(DeltaTime);
}

void UCharacterMoveState::SetNewSpeed(const float NewSpeed)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}
