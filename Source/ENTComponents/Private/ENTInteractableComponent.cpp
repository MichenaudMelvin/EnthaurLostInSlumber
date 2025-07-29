// Fill out your copyright notice in the Description page of Project Settings.

#include "ENTInteractableComponent.h"

UENTInteractableComponent::UENTInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UENTInteractableComponent::BeginDestroy()
{
	Super::BeginDestroy();

	if (!GetOwner())
	{
		return;
	}

	OnInteract.RemoveAll(GetOwner());
}

#pragma region Add/Remove Interactable

void UENTInteractableComponent::AddInteractable(UPrimitiveComponent* ComponentToAdd)
{
	if (ComponentToAdd == nullptr)
	{
		return;
	}

	InteractableComponentSet.Add(ComponentToAdd);
}

void UENTInteractableComponent::AddInteractables(TArray<UPrimitiveComponent*> ComponentsToAdd)
{
	InteractableComponentSet = TSet<UPrimitiveComponent*>(ComponentsToAdd);
}

void UENTInteractableComponent::RemoveInteractable(UPrimitiveComponent* ComponentToRemove)
{
	if (ComponentToRemove == nullptr)
	{
		return;
	}

	InteractableComponentSet.Remove(ComponentToRemove);
}

void UENTInteractableComponent::RemoveInteractables(TArray<UPrimitiveComponent*> ComponentsToRemove)
{
	for (UPrimitiveComponent* Comp : ComponentsToRemove)
	{
		RemoveInteractable(Comp);
	}
}

void UENTInteractableComponent::SelectPrimitive(UPrimitiveComponent* PrimitiveComponent)
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

void UENTInteractableComponent::Interact(APlayerController* PlayerController, APawn* Player)
{
	OnInteract.Broadcast(PlayerController, Player, TargetPrimitive);
}

#pragma endregion
