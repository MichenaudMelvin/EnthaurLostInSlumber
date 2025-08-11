// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/ENTCharacterStopState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"

UENTCharacterStopState::UENTCharacterStopState()
{
	StateID = EENTCharacterStateID::Stop;
}

void UENTCharacterStopState::StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	if (!Character)
	{
		StateMachine->ChangeState(EENTCharacterStateID::Fall);
		return;
	}

	Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	Character->GetCharacterMovement()->GravityScale = 0.0f;
}

void UENTCharacterStopState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	if (!Controller->GetPlayerInputs().bInputInteractTrigger)
	{
		Character->ReGrabSpike();
		Character->EjectCharacter(Character->GetActorForwardVector() * ImpulsionForce, true);
	}
}

