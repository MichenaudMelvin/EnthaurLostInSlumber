// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeakZoneInterface.h"
#include "GameFramework/Actor.h"
#include "Saves/WorldSaves/SaveGameElementInterface.h"
#include "RespawnTree.generated.h"

class UAkComponent;
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

	UFUNCTION()
	void Interact(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent);

	void SetRespawnPoint(AFirstPersonCharacter* Player, bool bSave);

	void SetActive();

	UPROPERTY(EditDefaultsOnly, Category = "RespawnTree")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(EditDefaultsOnly, Category = "RespawnTree")
	TObjectPtr<UStaticMeshComponent> TreeModel;

	UPROPERTY(EditDefaultsOnly, Category = "RespawnTree")
	TObjectPtr<class UInteractableComponent> Interaction;

	UPROPERTY(EditDefaultsOnly, Category = "RespawnTree")
	TObjectPtr<USceneComponent> RespawnPoint;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> Material;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkComponent> RespawnTreeNoises;

	UPROPERTY(EditAnywhere, Category = "Respawn", meta = (MakeEditWidget))
	FTransform RespawnTransform = FTransform(FVector(0.0f, 150.0f, 100.0f));

	FString LastCheckPointName;

	bool bIsActivated = false;

	virtual void OnEnterWeakZone_Implementation(bool bIsZoneActive) override;

	virtual void OnExitWeakZone_Implementation() override;

public:
	virtual FGameElementData& SaveGameElement(UWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FGameElementData& GameElementData) override;

	const FTransform& GetRespawnTransform() const {return RespawnTransform;}
};
