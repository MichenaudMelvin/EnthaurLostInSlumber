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
	void SetCurrentReceptacle(ANerveReceptacle* Receptacle) {CurrentAttachedReceptacle = Receptacle; }
	TObjectPtr<UInteractableComponent> GetInteractable() const { return InteractableComponent; }
	void DetachNerveBall();

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
	TObjectPtr<UInteractableComponent> InteractableComponent;

	UPROPERTY()
	FVector DefaultPosition;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CableLength;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CableMaxExtension;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DistanceNeededToPropulsion;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCableComponent> CableComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> NerveBall;

	FVector2f GetPropulsionForceMinMax() const
	{
		return PropulsionForceMinMax;
	}

private:
	UPROPERTY(EditAnywhere)
	FVector2f PropulsionForceMinMax;

	UPROPERTY()
	TObjectPtr<UPlayerToNervePhysicConstraint> PhysicConstraint;

	UPROPERTY()
	TObjectPtr<ANerveReceptacle> CurrentAttachedReceptacle;
};
