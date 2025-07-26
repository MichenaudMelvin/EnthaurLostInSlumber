// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterMoveState.h"
#include "AkComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Physics/SurfaceSettings.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Player/Camera/ViewBobbing.h"
#include "Saves/ENTSettingsSave.h"

UCharacterMoveState::UCharacterMoveState()
{
	bDoesMakeNoise = true;
}

void UCharacterMoveState::StateEnter_Implementation(const ECharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	LockMovement(false);

	Character->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	Character->GetCharacterMovement()->MaxAcceleration = MoveAcceleration;

	FootstepsDelay = FootstepsSpeedDelay;
	Character->GetFootstepsSoundComp()->SetSwitch(FootstepsSpeedNoise);
}

void UCharacterMoveState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	ApplyMovement(DeltaTime);
}

void UCharacterMoveState::ApplyMovement(float DeltaTime)
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

	PlayFootstepNoise(DeltaTime);
}

void UCharacterMoveState::PlayFootstepNoise(float DeltaTime)
{
	if (!FootstepsSpeedNoise)
	{
		return;
	}

	FootstepsDelay += DeltaTime;

	if (FootstepsDelay < FootstepsSpeedDelay)
	{
		return;
	}

	FootstepsDelay = 0.0f;

	FHitResult HitResult;
	bool bHit = Character->GroundTrace(HitResult);
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
		Character->GetFootstepsSoundComp()->SetSwitch(SurfaceNoise);
	}

	Character->GetFootstepsSoundComp()->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
}

void UCharacterMoveState::SetNewSpeed(const float NewSpeed)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}
