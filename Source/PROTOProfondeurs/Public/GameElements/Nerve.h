// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmberOre.h"
#include "WeakZoneInterface.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Saves/WorldSaves/SaveGameElementInterface.h"
#include "Nerve.generated.h"

USTRUCT(BlueprintType)
struct FNerveData : public FGameElementData
{
	GENERATED_BODY()

	/**
	 * @brief In local space
	 */
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> SplinePointsLocations;

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> ImpactNormals;
};

class USplineMeshComponent;
class USplineComponent;
class AFirstPersonController;
class ANerveReceptacle;
class UPlayerToNervePhysicConstraint;
class UInteractableComponent;

UCLASS()
class PROTOPROFONDEURS_API ANerve : public AActor, public IWeakZoneInterface, public ISaveGameElementInterface
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
	TObjectPtr<USplineComponent> SplineCable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cables")
	TArray<TObjectPtr<USplineMeshComponent>> SplineMeshes;

	UPROPERTY(EditAnywhere, Category = "Cables", meta = (ClampMin = 0.0f, Units = "cm"))
	float StartCableLength = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Cables", meta = (ClampMin = 0.0f, Units = "cm"))
	float CableMaxExtension = 1000.0f;

	void AddSplinePoint(const FVector& SpawnLocation, const ESplineCoordinateSpace::Type& CoordinateSpace = ESplineCoordinateSpace::World, bool bCreateSplineMesh = true, bool bAutoCorrect = true);

	void AddSplineMesh(const FVector& StartLocation, const FVector& EndLocation, const ESplineCoordinateSpace::Type& CoordinateSpace = ESplineCoordinateSpace::World);

	void RemoveLastSplinePoint();

	/**
	 * @brief 
	 * @param NewLocation Should be a world location
	 */
	void UpdateLastSplinePointLocation(const FVector& NewLocation);

	bool bShouldApplyCablePhysics = false;

	void ApplyCablesPhysics();

	bool CanCurrentCableBeRemoved();

	TArray<FVector> ImpactNormals;

	UPROPERTY(EditDefaultsOnly, Category = "Cables", meta = (ClampMin = 0.0f, Units = "cm"))
	float CableOffset = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Cables")
	TArray<TEnumAsByte<EObjectTypeQuery>> CableColliders;

	UPROPERTY(EditDefaultsOnly, Category = "Cables|Apperance")
	TObjectPtr<UStaticMesh> CableMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Cables|Apperance")
	TObjectPtr<UMaterial> CableMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Cables|Apperance")
	TEnumAsByte<ESplineMeshAxis::Type> CableForwardAxis = ESplineMeshAxis::Z;

	UPROPERTY(EditDefaultsOnly, Category = "Cables|Apperance")
	FVector2D CableScale = FVector2D(0.1f, 0.1f);

	/**
	 * @brief 
	 * @param bHardReset if true, clear all points without recreating the default state
	 */
	void ResetCables(bool bHardReset);

	UPROPERTY(EditDefaultsOnly, Category = "Cables|Retraction", meta = (ClampMin = 0.0f, ForceUnits = "cm/s"))
	float RetractionSpeed = 5000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Cables|Retraction")
	TObjectPtr<UCurveFloat> RetractionCurve;

	int32 RetractionIndex = -1;

	FTimeline RetractTimeline;

	UFUNCTION()
	void RetractCable(float Alpha);

	UFUNCTION()
	void FinishRetractCable();

public:
	FVector GetLastCableLocation(const ESplineCoordinateSpace::Type& CoordinateSpace = ESplineCoordinateSpace::World) const;

	float GetCableLength() const;

	float GetCableMaxExtension() const {return CableMaxExtension;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cables")
	FVector GetCableDirection() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cables")
	FVector GetCablePosition(float Percent, ESplineCoordinateSpace::Type CoordinateSpace = ESplineCoordinateSpace::World) const;

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

#pragma region Save

public:
	virtual FGameElementData& SaveGameElement(UWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FGameElementData& GameElementData) override;

#pragma endregion

protected:
	UPROPERTY()
	TObjectPtr<ANerveReceptacle> CurrentAttachedReceptacle;

public:
	void SetCurrentReceptacle(ANerveReceptacle* Receptacle);
};
