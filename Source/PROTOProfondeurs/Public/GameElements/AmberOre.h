// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmberOre.generated.h"

class UInteractableComponent;

UENUM()
enum class EAmberType : uint8
{
	NecroseAmber,
	WeakAmber
};

UCLASS()
class PROTOPROFONDEURS_API AAmberOre : public AActor
{
	GENERATED_BODY()

public:
	AAmberOre();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Amber")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Amber")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	TObjectPtr<UInteractableComponent> Interactable;

	UPROPERTY(EditDefaultsOnly, Category = "Amber|Material")
	TObjectPtr<UMaterialInterface> NecroseMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Amber|Material")
	TObjectPtr<UMaterialInterface> NecroseMaterialPickUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Amber|Material")
	TObjectPtr<UMaterialInterface> WeakMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Amber|Material")
	TObjectPtr<UMaterialInterface> WeakMaterialPickUp;

	UPROPERTY(EditInstanceOnly, Category = "Amber")
	EAmberType AmberType = EAmberType::NecroseAmber;

	UPROPERTY(EditInstanceOnly, Category = "Amber", meta = (ClampMin = 1))
	int OreAmount = 1;

	UFUNCTION()
	void OnInteract(APlayerController* Controller, APawn* Pawn);

	void UpdateMaterial(bool bPickedUp) const;
};
