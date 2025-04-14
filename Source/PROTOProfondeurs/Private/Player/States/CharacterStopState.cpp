// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterStopState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Player/States/CharacterStateMachine.h"

UCharacterStopState::UCharacterStopState()
{
	StateID = ECharacterStateID::Stop;
}

void UCharacterStopState::StateEnter_Implementation(const ECharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	if (!Character)
	{
		StateMachine->ChangeState(ECharacterStateID::Fall);
		return;
	}

	Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	Character->GetCharacterMovement()->GravityScale = 0.0f;
}

void UCharacterStopState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	if (Controller->GetPlayerInputs().bInputInteract)
	{
		Character->ReGrabSpike();
		Character->EjectCharacter(Character->GetActorForwardVector() * ImpulsionForce);
	}
}

