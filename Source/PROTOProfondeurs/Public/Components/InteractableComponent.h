// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"

#pragma region Delegates

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FKOnInteract, APlayerController*, Controller, APawn*, Pawn, UPrimitiveComponent*, InteractComponent);

#pragma endregion

UCLASS(ClassGroup = "Interactable", meta = (BlueprintSpawnableComponent))
class PROTOPROFONDEURS_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractableComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interactable")
	inline bool CheckComponent(const UPrimitiveComponent* ComponentToCheck) const
	{
		if (!ComponentToCheck)
		{
			return false;
		}

		return InteractableComponentSet.Contains(ComponentToCheck);
	}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction")
	FText InteractionName;

	UPROPERTY()
	TSet<UPrimitiveComponent*> InteractableComponentSet;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> TargetPrimitive;

#pragma region Add/Remove Interactable

public:
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void AddInteractable(UPrimitiveComponent* ComponentToAdd);

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void AddInteractables(TArray<UPrimitiveComponent*> ComponentsToAdd);

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void RemoveInteractable(UPrimitiveComponent* ComponentToRemove);

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void RemoveInteractables(TArray<UPrimitiveComponent*> ComponentsToRemove);

#pragma endregion

public:
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FKOnInteract OnInteract;

	void SelectPrimitive(UPrimitiveComponent* PrimitiveComponent);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact(APlayerController* PlayerController, APawn* Player);

	inline void SetInteractionName(const FText& NewInteractionName) {InteractionName = NewInteractionName;}

	FText GetInteractionName() const {return InteractionName;}
};
