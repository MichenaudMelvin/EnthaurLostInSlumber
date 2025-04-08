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
	ANerve();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> Root;

#pragma region Cables

protected:
	UPROPERTY()
	TArray<TObjectPtr<UCableComponent>> Cables;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CableLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CableMaxExtension = 1000.0f;

	void InitCable(const TObjectPtr<UCableComponent>& Cable) const;

	bool bShouldApplyCablePhysics = false;

	void ApplyCablesPhysics();

	UPROPERTY(EditDefaultsOnly, Category = "Cables", meta = (UIMin = 0.0f))
	float CableOffset = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Cables")
	TArray<TEnumAsByte<EObjectTypeQuery>> CableColliders;

public:
	FVector GetLastCableLocation() const;

	float GetCableLength() const;

	float GetCableMaxExtension() const {return CableMaxExtension;}

#pragma endregion

#pragma region NerveBall

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> NerveBall;

	FVector DefaultNervePosition = FVector::ZeroVector;

public:
	void AttachNerveBall(AActor* ActorToAttach);

	void DetachNerveBall();

	UStaticMeshComponent* GetNerveBall() const {return NerveBall;}

#pragma endregion

#pragma region Interaction

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInteractableComponent> InteractableComponent;

	UFUNCTION()
	void Interaction(APlayerController* PlayerController, APawn* Pawn);

public:
	TObjectPtr<UInteractableComponent> GetInteractable() const {return InteractableComponent;}

#pragma endregion

#pragma region Physics

protected:
	UPROPERTY()
	TObjectPtr<UPlayerToNervePhysicConstraint> PhysicConstraint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceNeededToPropulsion;

	UPROPERTY(EditAnywhere)
	FVector2D PropulsionForceMinMax;

public:
	float GetDistanceNeededToPropulsion() const {return DistanceNeededToPropulsion;}

	FVector2D GetPropulsionForceMinMax() const {return PropulsionForceMinMax;}

#pragma endregion

protected:
	UPROPERTY()
	TObjectPtr<ANerveReceptacle> CurrentAttachedReceptacle;

public:
	void SetCurrentReceptacle(ANerveReceptacle* Receptacle) {CurrentAttachedReceptacle = Receptacle;}
};
