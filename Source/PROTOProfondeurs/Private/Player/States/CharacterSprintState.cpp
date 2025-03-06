// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterSprintState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/CharacterSettings.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Player/States/CharacterStateMachine.h"

UCharacterSprintState::UCharacterSprintState()
{
	StateID = ECharacterStateID::Sprint;
	MoveSpeed = 1200.0f;
}

void UCharacterSprintState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	const bool bIsFalling = Character->GetCharacterMovement()->IsFalling();
	const UCharacterSettings* CharacterSettings = GetDefault<UCharacterSettings>();

	if(GetInputs().bInputJump && !bIsFalling)
	{
		StateMachine->ChangeState(ECharacterStateID::Jump);
	}

	else if(bIsFalling)
	{
		StateMachine->ChangeState(ECharacterStateID::Fall);
	}

	else if (!GetInputs().bInputSprint)
	{
		StateMachine->ChangeState(ECharacterStateID::Walk);
	}

	else if (GetInputs().bInputCrouch)
	{
		StateMachine->ChangeState(ECharacterStateID::Crouch);
	}

	else if (GetInputs().bInputInteract)
	{
		StateMachine->ChangeState(ECharacterStateID::Interact);
	}

	else if (GetInputs().InputMove.Length() < CharacterSettings->MoveInputThreshold)
	{
		StateMachine->ChangeState(ECharacterStateID::Idle);
	}
}
