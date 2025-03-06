// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmberOre.generated.h"

class UInteractableComponent;

UCLASS()
class PROTOPROFONDEURS_API AAmberOre : public AActor
{
	GENERATED_BODY()

public:
	AAmberOre();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Amber")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Amber")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Amber")
	TObjectPtr<UInteractableComponent> Interactable;

	UFUNCTION()
	void OnInteract(APlayerController* Controller, APawn* Pawn);
};
