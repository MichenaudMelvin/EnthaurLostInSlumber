// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Saves/WorldSaves/SaveGameElementInterface.h"
#include "AmberOre.generated.h"

class UAkComponent;
class UBoxComponent;

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

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Amber")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Amber")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Amber")
	TObjectPtr<UStaticMeshComponent> AmberMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeakZone")
	TObjectPtr<UBoxComponent> MeshInteraction;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	TObjectPtr<UInteractableComponent> Interactable;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	TObjectPtr<UAkComponent> AmberOreNoises;

	UPROPERTY(EditAnywhere, Category = "Amber")
	TObjectPtr<UStaticMesh> SourceMesh;

	UPROPERTY(EditInstanceOnly, Category = "Amber")
	EAmberType AmberType = EAmberType::NecroseAmber;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	float TargetAmberHeight = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	float AmberAnimSpeed = 1.5f;

	UPROPERTY(EditInstanceOnly, Category = "Amber", meta = (ClampMin = 1))
	uint8 OreAmount = 1;

	UFUNCTION()
	void OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent);

public:
	virtual FGameElementData& SaveGameElement(UWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FGameElementData& GameElementData) override;
};
