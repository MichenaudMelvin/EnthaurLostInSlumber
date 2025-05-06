// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeakZoneInterface.h"
#include "GameFramework/Actor.h"
#include "RespawnTree.generated.h"

class UAkAudioEvent;

UCLASS()
class PROTOPROFONDEURS_API ARespawnTree : public AActor, public IWeakZoneInterface
{
	GENERATED_BODY()

public:
	ARespawnTree();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void ActivateRespawn(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent);

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

	UPROPERTY()
	bool bIsActivated;

	UPROPERTY(EditAnywhere)
	float lightLevel;

	virtual void OnEnterWeakZone_Implementation(bool bIsZoneActive) override;

	virtual void OnExitWeakZone_Implementation() override;
};
