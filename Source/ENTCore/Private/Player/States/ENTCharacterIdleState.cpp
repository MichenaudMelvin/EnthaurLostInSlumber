// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/ENTCharacterIdleState.h"
#include "Config/ENTCoreConfig.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"

UENTCharacterIdleState::UENTCharacterIdleState()
{
	StateID = EENTCharacterStateID::Idle;
}

void UENTCharacterIdleState::StateInit(UENTCharacterStateMachine* InStateMachine)
{
	Super::StateInit(InStateMachine);
}

void UENTCharacterIdleState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	const UENTCoreConfig* CoreConfig = GetDefault<UENTCoreConfig>();

	if (GetInputs().bInputJump)
	{
		StateMachine->ChangeState(EENTCharacterStateID::Jump);
	}

	else if (GetInputs().bInputCrouch)
	{
		StateMachine->ChangeState(EENTCharacterStateID::Crouch);
	}

	else if (GetInputs().InputMove.Length() > CoreConfig->MoveInputThreshold)
	{
		StateMachine->ChangeState(EENTCharacterStateID::Walk);
	}
}
