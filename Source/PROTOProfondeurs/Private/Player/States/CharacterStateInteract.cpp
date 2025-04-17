// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStateInteract.h"
#include "Components/InteractableComponent.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Player/States/CharacterStateMachine.h"

UCharacterStateInteract::UCharacterStateInteract()
{
	StateID = ECharacterStateID::Interact;
}

void UCharacterStateInteract::StateEnter_Implementation(const ECharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	UInteractableComponent* InteractableComponent = Character->GetCurrentInteractable();
	if (InteractableComponent == nullptr)
	{
		StateMachine->ChangeState(ECharacterStateID::Idle);
		return;
	}

	Duration = 0.0f;
	InteractableComponent->Interact(Character->GetPlayerController(), Character);
}

void UCharacterStateInteract::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	Duration += DeltaTime;
	if (Duration >= InteractionDuration)
	{
		StateMachine->ChangeState(ECharacterStateID::Idle);
	}
}
