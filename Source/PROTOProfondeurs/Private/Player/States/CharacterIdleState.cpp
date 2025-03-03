// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterIdleState.h"
#include "Player/FirstPersonController.h"
#include "Player/States/CharacterStateMachine.h"
#include "Settings/DefaultInputSettings.h"

UCharacterIdleState::UCharacterIdleState()
{
	StateID = ECharacterStateID::Idle;
	bCheckGround = true;
}

void UCharacterIdleState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	const UDefaultInputSettings* InputSettings = GetDefault<UDefaultInputSettings>();

	if (GetInputs().bInputJump)
	{
		StateMachine->ChangeState(ECharacterStateID::Jump);
	}

	else if (GetInputs().bInputCrouch)
	{
		StateMachine->ChangeState(ECharacterStateID::Crouch);
	}

	else if (GetInputs().bInputInteract)
	{
		StateMachine->ChangeState(ECharacterStateID::Interact);
	}

	else if (GetInputs().bInputTakeAmber)
	{
		StateMachine->ChangeState(ECharacterStateID::TakeAmber);
	}

	else if (GetInputs().InputMove.Length() > InputSettings->MoveInputThreshold)
	{
		StateMachine->ChangeState(ECharacterStateID::Walk);
	}
}
