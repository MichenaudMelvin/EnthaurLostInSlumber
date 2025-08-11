// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/ENTCharacterMoveState.h"
#include "AkComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Config/ENTCoreConfig.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/Camera/ENTViewBobbing.h"
#include "Saves/ENTSettingsSave.h"

UENTCharacterMoveState::UENTCharacterMoveState()
{
	bDoesMakeNoise = true;
}

void UENTCharacterMoveState::StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	LockMovement(false);

	Character->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	Character->GetCharacterMovement()->MaxAcceleration = MoveAcceleration;

	FootstepsDelay = FootstepsSpeedDelay;
	Character->GetFootstepsSoundComp()->SetSwitch(FootstepsSpeedNoise);
}

void UENTCharacterMoveState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	ApplyMovement(DeltaTime);
}

void UENTCharacterMoveState::ApplyMovement(float DeltaTime)
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

void UENTCharacterMoveState::PlayFootstepNoise(float DeltaTime)
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

	const UENTCoreConfig* CoreConfig = GetDefault<UENTCoreConfig>();
	if (!CoreConfig)
	{
		return;
	}

	const UAkSwitchValue* SurfaceNoise = CoreConfig->FindNoise(HitResult.PhysMaterial->SurfaceType);
	if (SurfaceNoise)
	{
		Character->GetFootstepsSoundComp()->SetSwitch(SurfaceNoise);
	}

	Character->GetFootstepsSoundComp()->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
}

void UENTCharacterMoveState::SetNewSpeed(const float NewSpeed)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}
