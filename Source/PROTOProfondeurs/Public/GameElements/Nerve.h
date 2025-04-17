// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CableComponent.h"
#include "WeakZoneInterface.h"
#include "GameFramework/Actor.h"
#include "Nerve.generated.h"

class AFirstPersonController;
class ANerveReceptacle;
class UPlayerToNervePhysicConstraint;
class UInteractableComponent;

UCLASS()
class PROTOPROFONDEURS_API ANerve : public AActor, public IWeakZoneInterface
{
	GENERATED_BODY()

public:
	ANerve();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USceneComponent> Root;

#pragma region Cables

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cables")
	TArray<TObjectPtr<UCableComponent>> Cables;

	UPROPERTY(EditAnywhere, Category = "Cables", meta = (ClampMin = 0.0f, Units = "cm"))
	float StartCableLength = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Cables", meta = (ClampMin = 0.0f, Units = "cm"))
	float CableMaxExtension = 1000.0f;

	void InitCable(const TObjectPtr<UCableComponent>& Cable) const;

	bool bShouldApplyCablePhysics = false;

	void ApplyCablesPhysics();

	bool CanCurrentCableBeRemoved(UCableComponent* CurrentCable, UCableComponent* LastCable);

	FVector LastImpactNormal = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = "Cables", meta = (ClampMin = 0.0f, Units = "cm"))
	float CableOffset = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Cables")
	TArray<TEnumAsByte<EObjectTypeQuery>> CableColliders;

	UPROPERTY(EditDefaultsOnly, Category = "Cables")
	TObjectPtr<UMaterial> CableMaterial;

public:
	FVector GetLastCableLocation() const;

	float GetCableLength() const;

	float GetCableMaxExtension() const {return CableMaxExtension;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cables")
	FVector GetCableDirection() const;

#pragma endregion

#pragma region NerveBall

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> NerveBall;

	FVector DefaultNervePosition = FVector::ZeroVector;

public:
	void AttachNerveBall(AActor* ActorToAttach);

	void DetachNerveBall();

	UStaticMeshComponent* GetNerveBall() const {return NerveBall;}

	/**
	 * @brief Is attached to something else
	 * @return 
	 */
	bool IsNerveBallAttached() const;

#pragma endregion

#pragma region Interaction

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInteractableComponent> InteractableComponent;

	UFUNCTION()
	void Interaction(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent);

	UPROPERTY()
	TObjectPtr<AFirstPersonController> PlayerController;

public:
	TObjectPtr<UInteractableComponent> GetInteractable() const {return InteractableComponent;}

#pragma endregion

#pragma region Physics

protected:
	UPROPERTY()
	TObjectPtr<UPlayerToNervePhysicConstraint> PhysicConstraint;

	UPROPERTY(EditAnywhere, Category = "Physics", meta = (ClampMin = 0.0f, Units = "cm"))
	float DistanceNeededToPropulsion = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Physics")
	FFloatRange PropulsionForceRange = FFloatRange(500.0f, 1000.0f);

public:
	float GetDistanceNeededToPropulsion() const {return DistanceNeededToPropulsion;}

	FFloatRange GetPropulsionForceRange() const {return PropulsionForceRange;}

#pragma endregion

#pragma region WeakZone

private:
	virtual void OnEnterWeakZone_Implementation(bool bIsZoneActive) override;

	virtual void OnExitWeakZone_Implementation() override;

#pragma endregion

protected:
	UPROPERTY()
	TObjectPtr<ANerveReceptacle> CurrentAttachedReceptacle;

public:
	void SetCurrentReceptacle(ANerveReceptacle* Receptacle) {CurrentAttachedReceptacle = Receptacle;}
};
