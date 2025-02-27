// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterMoveState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"

UCharacterMoveState::UCharacterMoveState()
{
	bCheckGround = true;
}

void UCharacterMoveState::StateEnter_Implementation(const ECharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	Character->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	Character->GetCharacterMovement()->MaxAcceleration = MoveAcceleration;
}

void UCharacterMoveState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	Character->AddMovementInput(Character->GetActorForwardVector(), GetInputs().InputMove.Y);
	Character->AddMovementInput(Character->GetActorRightVector(), GetInputs().InputMove.X);
}
