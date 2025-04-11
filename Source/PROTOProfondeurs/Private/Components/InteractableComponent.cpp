// Fill out your copyright notice in the Description page of Project Settings.

#include "PROTOProfondeurs/Public/Components/InteractableComponent.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

#pragma region Add/Remove Interactable

void UInteractableComponent::AddInteractable(UPrimitiveComponent* ComponentToAdd)
{
	if (ComponentToAdd == nullptr)
	{
		return;
	}

	InteractableComponentSet.Add(ComponentToAdd);
}

void UInteractableComponent::AddInteractables(TArray<UPrimitiveComponent*> ComponentsToAdd)
{
	InteractableComponentSet = TSet<UPrimitiveComponent*>(ComponentsToAdd);
}

void UInteractableComponent::RemoveInteractable(UPrimitiveComponent* ComponentToRemove)
{
	if (ComponentToRemove == nullptr)
	{
		return;
	}

	InteractableComponentSet.Remove(ComponentToRemove);
}

void UInteractableComponent::RemoveInteractables(TArray<UPrimitiveComponent*> ComponentsToRemove)
{
	for (UPrimitiveComponent* Comp : ComponentsToRemove)
	{
		RemoveInteractable(Comp);
	}
}

void UInteractableComponent::SelectPrimitive(UPrimitiveComponent* PrimitiveComponent)
{
	if (!PrimitiveComponent)
	{
		return;
	}

	if (!InteractableComponentSet.Contains(PrimitiveComponent))
	{
		return;
	}

	TargetPrimitive = PrimitiveComponent;
}

void UInteractableComponent::Interact(APlayerController* PlayerController, APawn* Player)
{
	OnInteract.Broadcast(PlayerController, Player, TargetPrimitive);
}

#pragma endregion
