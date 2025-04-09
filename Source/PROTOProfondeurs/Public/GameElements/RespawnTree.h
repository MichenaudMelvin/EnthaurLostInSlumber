// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RespawnTree.generated.h"

UCLASS()
class PROTOPROFONDEURS_API ARespawnTree : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARespawnTree();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void ActivateRespawn(APlayerController* PlayerController, APawn* Pawn);
	
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

	UPROPERTY()
	bool bIsActivated;
	
	UPROPERTY(EditAnywhere)
	float lightLevel;
};
