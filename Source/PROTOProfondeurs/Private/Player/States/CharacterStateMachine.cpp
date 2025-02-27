// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterStateMachine.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/States/CharacterState.h"

void UCharacterStateMachine::InitStateMachine(AFirstPersonCharacter* InCharacter)
{
	Character = InCharacter;
	StateList = Character->GetStates();

	for (UCharacterState* State : StateList)
	{
		if (State == nullptr)
		{
			continue;
		}

		State->StateInit(this);
	}

	ChangeState(ECharacterStateID::Idle);
}

void UCharacterStateMachine::TickStateMachine(float DeltaTime)
{
	if(CurrentState == nullptr)
	{
		return;
	}

#if WITH_EDITOR
	if (Character->DebugStates())
	{
		const FString Message = FString::Printf(TEXT("CurrentState: %d (Class: %s)"), CurrentStateID, *CurrentState->GetClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, Message);
	}
#endif

	CurrentState->StateTick(DeltaTime);
}

void UCharacterStateMachine::ChangeState(ECharacterStateID NextStateID)
{
	UCharacterState* NextState = FindState(NextStateID);

	if(NextState == nullptr)
	{
		return;
	}

	if(CurrentState != nullptr)
	{
		CurrentState->StateExit(NextStateID);
	}

	const ECharacterStateID PreviousStateID = CurrentStateID;
	CurrentStateID = NextStateID;
	CurrentState = NextState;

	if(CurrentState == nullptr)
	{
		return;
	}

	CurrentState->StateEnter(PreviousStateID);
}

UCharacterState* UCharacterStateMachine::FindState(ECharacterStateID StateID) const
{
	for (UCharacterState* State : StateList)
	{
		if(StateID == State->GetStateID())
		{
			return State;
		}
	}

	return nullptr;
}

UCharacterState* UCharacterStateMachine::FindStateByClass(TSubclassOf<UCharacterState> StateClass)
{
	for (UCharacterState* State : StateList)
	{
		if(State->IsA(StateClass))
		{
			return State;
		}
	}

	return nullptr;
}
