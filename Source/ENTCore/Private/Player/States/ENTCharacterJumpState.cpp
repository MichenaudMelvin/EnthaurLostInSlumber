// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/ENTCharacterJumpState.h"
#include "AkComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"

UENTCharacterJumpState::UENTCharacterJumpState()
{
	StateID = EENTCharacterStateID::Jump;
	MoveSpeed = 400.0f;
}

void UENTCharacterJumpState::StateInit(UENTCharacterStateMachine* InStateMachine)
{
	Super::StateInit(InStateMachine);

	JumpVelocity = (2.0f * MaxHeight) / Duration;
	JumpGravity = (-2.0f * MaxHeight) / FMath::Pow(Duration, 2);
}

void UENTCharacterJumpState::StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	PreviousGravityScale = Character->GetCharacterMovement()->GravityScale;

	Character->GetCharacterMovement()->GravityScale = 0.0f;
	Character->GetCharacterMovement()->AirControl = AirControl;
	Character->GetCharacterMovement()->BrakingDecelerationFalling = FallingDeceleration;

	Character->GetCharacterMovement()->Velocity.Z = JumpVelocity;
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	Character->GetFootstepsSoundComp()->PostAkEvent(JumpNoise, 0, FOnAkPostEventCallback());
}

void UENTCharacterJumpState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	Character->GetCharacterMovement()->Velocity.Z += JumpGravity * DeltaTime;

	if (!GetInputs().bInputJump || Character->GetCharacterMovement()->Velocity.Z <= 0.0f)
	{
		StateMachine->ChangeState(EENTCharacterStateID::Fall);
	}
}

void UENTCharacterJumpState::StateExit_Implementation(const EENTCharacterStateID& NextStateID)
{
	Super::StateExit_Implementation(NextStateID);

	Character->GetCharacterMovement()->GravityScale = PreviousGravityScale;
}
