// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterJumpState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Player/States/CharacterStateMachine.h"

UCharacterJumpState::UCharacterJumpState()
{
	StateID = ECharacterStateID::Jump;
	MoveSpeed = 400.0f;
	bCheckGround = false;
}

void UCharacterJumpState::StateInit(UCharacterStateMachine* InStateMachine)
{
	Super::StateInit(InStateMachine);

	JumpVelocity = (2.0f * MaxHeight) / Duration;
	JumpGravity = (-2.0f * MaxHeight) / FMath::Pow(Duration, 2);
}

void UCharacterJumpState::StateEnter_Implementation(const ECharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	PreviousGravityScale = Character->GetCharacterMovement()->GravityScale;

	Character->GetCharacterMovement()->AirControl = AirControl;
	Character->GetCharacterMovement()->GravityScale = 0.0f;

	Character->GetCharacterMovement()->Velocity.Z = JumpVelocity;
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}

void UCharacterJumpState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	Character->GetCharacterMovement()->Velocity.Z += JumpGravity * DeltaTime;

	if (!GetInputs().bInputJump || Character->GetCharacterMovement()->Velocity.Z <= 0.0f)
	{
		StateMachine->ChangeState(ECharacterStateID::Fall);
	}
}

void UCharacterJumpState::StateExit_Implementation(const ECharacterStateID& NextStateID)
{
	Super::StateExit_Implementation(NextStateID);

	Character->GetCharacterMovement()->GravityScale = PreviousGravityScale;
}
