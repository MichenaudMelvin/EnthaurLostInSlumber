// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Saves/WorldSaves/SaveGameElementInterface.h"
#include "AmberOre.generated.h"

USTRUCT(BlueprintType)
struct FAmberOreData : public FGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "AmberOre")
	uint8 CurrentOreAmount = 0;
};

class UInteractableComponent;

UENUM()
enum class EAmberType : uint8
{
	NecroseAmber,
	WeakAmber
};

UCLASS()
class PROTOPROFONDEURS_API AAmberOre : public AActor, public ISaveGameElementInterface
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
	uint8 OreAmount = 1;

	UFUNCTION()
	void OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent);

	void UpdateMaterial(bool bPickedUp) const;

public:
	virtual FGameElementData& SaveGameElement(UWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FGameElementData& GameElementData) override;
};
