// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterSlideState.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/States/CharacterStateMachine.h"
#include "Physics/TracePhysicsSettings.h"

UCharacterSlideState::UCharacterSlideState()
{
	StateID = ECharacterStateID::Slide;
	bCheckGround = true;
	MoveSpeed = 2000.0f;
	MoveAcceleration = 3000.0f;
}

void UCharacterSlideState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	float SlopeAngle;
	FVector SlopeNormal;
	if (!Character->GetSlopeProperties(SlopeAngle, SlopeNormal))
	{
		return;
	}

	Character->AddMovementInput(SlopeNormal, SlopeAngle * SlipperyFactor);
}

void UCharacterSlideState::OnWalkOnNewSurface_Implementation(const TEnumAsByte<EPhysicalSurface>& NewSurface)
{
	const UTracePhysicsSettings* StateSettings = GetDefault<UTracePhysicsSettings>();

	if (NewSurface != StateSettings->SlipperySurface)
	{
		StateMachine->ChangeState(ECharacterStateID::Idle);
	}
}
