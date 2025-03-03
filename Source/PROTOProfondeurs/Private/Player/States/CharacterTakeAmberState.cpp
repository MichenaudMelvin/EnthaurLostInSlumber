// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterTakeAmberState.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/States/CharacterStateMachine.h"

UCharacterTakeAmberState::UCharacterTakeAmberState()
{
	StateID = ECharacterStateID::TakeAmber;
}

void UCharacterTakeAmberState::StateEnter_Implementation(const ECharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	if (!Character->CantTakeAmber())
	{
		StateMachine->ChangeState(ECharacterStateID::Idle);
	}
}

void UCharacterTakeAmberState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	
}
