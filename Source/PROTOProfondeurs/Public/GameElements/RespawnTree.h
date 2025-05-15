// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeakZoneInterface.h"
#include "GameFramework/Actor.h"
#include "Saves/WorldSaves/SaveGameElementInterface.h"
#include "RespawnTree.generated.h"

class AFirstPersonCharacter;
class UAkAudioEvent;

USTRUCT(BlueprintType)
struct FRespawnTreeData : public FGameElementData
{
	GENERATED_BODY()

	UPROPERTY()
	bool bIsActive = false;
};

UCLASS()
class PROTOPROFONDEURS_API ARespawnTree : public AActor, public IWeakZoneInterface, public ISaveGameElementInterface
{
	GENERATED_BODY()

public:
	ARespawnTree();

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

private:
	UFUNCTION()
	void Interact(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent);

	void SetRespawnPoint(AFirstPersonCharacter* Player, bool bSave);

	void SetActive();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> TreeModel;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UInteractableComponent> Interaction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> RespawnPoint;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UPointLightComponent> Light;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> BulbMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkAudioEvent> ActivationNoise;

	FString LastCheckPointName;

	bool bIsActivated = false;

	UPROPERTY(EditAnywhere)
	float lightLevel;

	virtual void OnEnterWeakZone_Implementation(bool bIsZoneActive) override;

	virtual void OnExitWeakZone_Implementation() override;

public:
	virtual void SaveGameElement(UWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FGameElementData& GameElementData) override;
};
