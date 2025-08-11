// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/ENTCharacterSprintState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Config/ENTCoreConfig.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"

UENTCharacterSprintState::UENTCharacterSprintState()
{
	StateID = EENTCharacterStateID::Sprint;
	MoveSpeed = 1200.0f;
}

void UENTCharacterSprintState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	const bool bIsFalling = Character->GetCharacterMovement()->IsFalling();
	const UENTCoreConfig* CoreConfig = GetDefault<UENTCoreConfig>();

	if(GetInputs().bInputJump && !bIsFalling)
	{
		StateMachine->ChangeState(EENTCharacterStateID::Jump);
	}

	else if(bIsFalling)
	{
		StateMachine->ChangeState(EENTCharacterStateID::Fall);
	}

	else if (!GetInputs().bInputSprint)
	{
		StateMachine->ChangeState(EENTCharacterStateID::Walk);
	}

	else if (GetInputs().bInputCrouch)
	{
		StateMachine->ChangeState(EENTCharacterStateID::Crouch);
	}

	else if (GetInputs().InputMove.Length() < CoreConfig->MoveInputThreshold)
	{
		StateMachine->ChangeState(EENTCharacterStateID::Idle);
	}
}
