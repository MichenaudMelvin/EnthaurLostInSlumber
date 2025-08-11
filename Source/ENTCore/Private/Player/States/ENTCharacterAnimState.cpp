// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/ENTCharacterAnimState.h"
#include "ENTToolStatics.h"
#include "Player/States/ENTCharacterStateMachine.h"

UENTCharacterAnimState::UENTCharacterAnimState()
{
	StateID = EENTCharacterStateID::Anim;
}

void UENTCharacterAnimState::StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	if (PreviousStateID == StateID)
	{
		PreviousState = EENTCharacterStateID::Idle;
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
	AnimDuration = UENTToolStatics::GetAnimLength(StateAnimation);
}

void UENTCharacterAnimState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	StateDuration += DeltaTime;
	if (StateDuration < AnimDuration)
	{
		return;
	}

	StateMachine->ChangeState(PreviousState);
}
