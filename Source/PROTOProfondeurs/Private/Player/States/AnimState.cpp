// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/AnimState.h"
#include "ToolStatics.h"
#include "Player/States/CharacterStateMachine.h"

UAnimState::UAnimState()
{
	StateID = ECharacterStateID::Anim;
}

void UAnimState::StateEnter_Implementation(const ECharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	if (PreviousStateID == StateID)
	{
		PreviousState = ECharacterStateID::Idle;
	}
	else
	{
		PreviousState = PreviousStateID;
	}

	if (!StateAnimation)
	{
		StateMachine->ChangeState(PreviousState);
		return;
	}

	StateDuration = 0.0f;
	AnimDuration = UToolStatics::GetAnimLength(StateAnimation);
}

void UAnimState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	StateDuration += DeltaTime;
	if (StateDuration < AnimDuration)
	{
		return;
	}

	StateMachine->ChangeState(PreviousState);
}
