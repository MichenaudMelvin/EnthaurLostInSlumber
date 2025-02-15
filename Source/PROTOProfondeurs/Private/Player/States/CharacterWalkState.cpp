// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterWalkState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Player/States/CharacterStateMachine.h"

UCharacterWalkState::UCharacterWalkState()
{
	StateID = ECharacterStateID::Walk;
}

void UCharacterWalkState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	const bool bIsFalling = Character->GetCharacterMovement()->IsFalling();

	if(GetInputs().bInputJump && !bIsFalling)
	{
		StateMachine->ChangeState(ECharacterStateID::Jump);
	}

	else if(bIsFalling)
	{
		StateMachine->ChangeState(ECharacterStateID::Fall);
	}

	else if (GetInputs().bInputSprint)
	{
		StateMachine->ChangeState(ECharacterStateID::Sprint);
	}

	else if (GetInputs().bInputCrouch)
	{
		StateMachine->ChangeState(ECharacterStateID::Crouch);
	}

	else if (GetInputs().InputMove.Length() == 0.0f)
	{
		StateMachine->ChangeState(ECharacterStateID::Idle);
	}
}
