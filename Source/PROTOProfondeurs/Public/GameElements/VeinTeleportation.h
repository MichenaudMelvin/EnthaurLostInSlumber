// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VeinTeleportation.generated.h"

UCLASS()
class PROTOPROFONDEURS_API AVeinTeleportation : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractComponent);
	// Sets default values for this actor's properties
	AVeinTeleportation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UChildActorComponent> VeinMesh;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> EntryPoint;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UInteractableComponent> Interaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> MapToLoad;
};
