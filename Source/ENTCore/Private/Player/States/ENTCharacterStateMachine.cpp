// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/ENTCharacterStateMachine.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/States/ENTCharacterState.h"

void UENTCharacterStateMachine::InitStateMachine(AENTDefaultCharacter* InCharacter)
{
	Character = InCharacter;
	StateList = Character->GetStates();

	for (UENTCharacterState* State : StateList)
	{
		if (State == nullptr)
		{
			continue;
		}

		State->StateInit(this);
	}

	ChangeState(EENTCharacterStateID::Idle);
}

void UENTCharacterStateMachine::TickStateMachine(float DeltaTime)
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

UENTCharacterState* UENTCharacterStateMachine::ChangeState(EENTCharacterStateID NextStateID)
{
	UENTCharacterState* NextState = FindState(NextStateID);

	if(NextState == nullptr)
	{
		return nullptr;
	}

	if (NextState->IsLocked())
	{
		return nullptr;
	}

	if(CurrentState != nullptr)
	{
		CurrentState->StateExit(NextStateID);
	}

	const EENTCharacterStateID PreviousStateID = CurrentStateID;
	CurrentStateID = NextStateID;
	CurrentState = NextState;

	if(CurrentState == nullptr)
	{
		return nullptr;
	}

	CurrentState->StateEnter(PreviousStateID);
	return NextState;
}

UENTCharacterState* UENTCharacterStateMachine::FindState(EENTCharacterStateID StateID) const
{
	for (UENTCharacterState* State : StateList)
	{
		if(StateID == State->GetStateID())
		{
			return State;
		}
	}

	return nullptr;
}

void UENTCharacterStateMachine::LockState(EENTCharacterStateID StateToLock, bool bLock) const
{
	UENTCharacterState* CharacterState = FindState(StateToLock);
	if (!CharacterState)
	{
		return;
	}

	CharacterState->LockState(bLock);
}

void UENTCharacterStateMachine::LockAllStates(bool bLock)
{
	for (UENTCharacterState* State : StateList)
	{
		if (!State)
		{
			continue;
		}

		State->LockState(bLock);
	}
}

UENTCharacterState* UENTCharacterStateMachine::FindStateByClass(TSubclassOf<UENTCharacterState> StateClass)
{
	for (UENTCharacterState* State : StateList)
	{
		if(State->IsA(StateClass))
		{
			return State;
		}
	}

	return nullptr;
}
