// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CableComponent.h"
#include "GameFramework/Actor.h"
#include "Nerve.generated.h"

class ANerveReceptacle;
class UPlayerToNervePhysicConstraint;
class UInteractableComponent;

UCLASS()
class PROTOPROFONDEURS_API ANerve : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANerve();

	float GetCableLength() const { return CableLength; }
	float GetCableMaxExtension() const { return CableMaxExtension; }
	TObjectPtr<UInteractableComponent> GetInteractable() const { return InteractableComponent; }

	UFUNCTION()
	void Interaction(APlayerController* PlayerController, APawn* Pawn);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCableComponent> CableComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> NerveBall;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInteractableComponent> InteractableComponent;
	
	UPROPERTY(EditAnywhere)
	float CableLength;
	UPROPERTY(EditAnywhere)
	float CableMaxExtension;

	UPROPERTY()
	TObjectPtr<UPlayerToNervePhysicConstraint> PhysicConstraint;
};
