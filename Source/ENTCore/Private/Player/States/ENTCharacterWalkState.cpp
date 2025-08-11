// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/ENTCharacterWalkState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"

UENTCharacterWalkState::UENTCharacterWalkState()
{
	StateID = EENTCharacterStateID::Walk;
}

void UENTCharacterWalkState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	if(GetInputs().bInputJump && !IsFalling())
	{
		StateMachine->ChangeState(EENTCharacterStateID::Jump);
	}

	else if(IsFalling())
	{
		StateMachine->ChangeState(EENTCharacterStateID::Fall);
	}

	else if (GetInputs().bInputSprint)
	{
		StateMachine->ChangeState(EENTCharacterStateID::Sprint);
	}

	else if (GetInputs().bInputCrouch)
	{
		StateMachine->ChangeState(EENTCharacterStateID::Crouch);
	}

	else if (GetInputs().InputMove.Length() == 0.0f)
	{
		StateMachine->ChangeState(EENTCharacterStateID::Idle);
	}
}
