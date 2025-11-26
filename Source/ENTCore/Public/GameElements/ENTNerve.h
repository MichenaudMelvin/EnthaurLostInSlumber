// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTWeakZoneInterface.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Saves/WorldSaves/ENTSaveGameElementInterface.h"
#include "ENTNerve.generated.h"

class UAkComponent;
class UAkAudioEvent;
class AENTDefaultCharacter;

class UAkRtpc;
class USplineMeshComponent;
class USplineComponent;
class AENTDefaultPlayerController;
class AENTNerveReceptacle;
class UENTPhysicConstraint;
class UENTInteractableComponent;

UCLASS()
class ENTCORE_API AENTNerve : public AActor, public IENTWeakZoneInterface, public IENTSaveGameElementInterface
{
	GENERATED_BODY()

public:
	AENTNerve();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostInitProperties() override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cables", meta = (ClampMin = 0.0f, Units = "cm"))
	float CableMaxExtension = 1000.0f;

	bool bIsHolding = false;

	void AddSplinePoint(const FVector& SpawnLocation, const ESplineCoordinateSpace::Type& CoordinateSpace = ESplineCoordinateSpace::World, bool bAutoCorrect = true) const;

	void RemoveLastSplinePoint() const;

	void AddSplineMesh(bool bMakeNoise);

	void RemoveSplineMesh();

	void UpdateSplineMeshes(bool bUseNerveBallAsEndPoint, bool bMakeNoise);

	void BuildSplineMeshes();

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

	UPROPERTY(EditAnywhere, Category = "Cables|Apperance")
	TObjectPtr<UStaticMesh> NerveMesh;

	UPROPERTY(EditAnywhere, Category = "Cables|Apperance")
	TObjectPtr<UStaticMesh> LigamentMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> TargetMesh;

	UPROPERTY(EditAnywhere, Category = "Cables|Apperance")
	TObjectPtr<UMaterial> NerveMaterial;

	UPROPERTY(EditAnywhere, Category = "Cables|Apperance")
	TObjectPtr<UMaterial> BaseLigamentMaterial;

	UPROPERTY(EditAnywhere, Category = "Cables|Apperance")
	TObjectPtr<UMaterial> StretchedLigamentMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicStretchedLigamentMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Cables|Apperance")
	TEnumAsByte<ESplineMeshAxis::Type> CableForwardAxis = ESplineMeshAxis::Z;

	UPROPERTY(VisibleDefaultsOnly, Category = "Cables|Apperance", meta = (Units = "cm"))
	float SingleCableLength = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Cables|Apperance")
	FVector2D CableScale = FVector2D(1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Cables|Apperance")
	float TransparencyDistance = 250.f;

	UPROPERTY(EditDefaultsOnly, Category = "Cables|Apperance")
	float MaxVibrationStrength = 1.5f;

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

	float GetNerveBallLength() const;

	float GetCableMaxExtension() const {return CableMaxExtension;}

	float GetMaxVibrationStrength() const {return MaxVibrationStrength;}

	FVector GetStartCableLocation() const {return SplineCable->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cables")
	FVector GetCableDirection() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cables")
	FVector GetCablePosition(float Percent, ESplineCoordinateSpace::Type CoordinateSpace = ESplineCoordinateSpace::World) const;

	TObjectPtr<UMaterialInstanceDynamic> GetDynamicCableStretchedMaterial() const {return DynamicStretchedLigamentMaterial;}

#pragma endregion

#pragma region NerveBall

protected:
	UFUNCTION()
	void ForceDetachNerveBallFromPlayer();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nerve")
	TObjectPtr<UStaticMeshComponent> NerveBall;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nerve")
	TObjectPtr<UStaticMeshComponent> CorruptNerveBlocker;

	UPROPERTY(EditDefaultsOnly, Category = "Nerve")
	TObjectPtr<UStaticMesh> CorruptNerveBlockerMesh;

	UPROPERTY(EditDefaultsOnly, Category = "NerveBall|Apperance")
	TObjectPtr<UStaticMesh> LigamentBallMesh;

	UPROPERTY(EditDefaultsOnly, Category = "NerveBall|Apperance")
	TObjectPtr<UStaticMesh> NerveBallMesh;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nerve")
	TObjectPtr<UAkComponent> NerveStretchComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nerve")
	TObjectPtr<UAkRtpc> NerveStretchRtpc;
	
	FVector DefaultNervePosition = FVector::ZeroVector;

public:
	void AttachNerveBall(AActor* ActorToAttach);

	void DetachNerveBall(bool bForceDetachment);

	UStaticMeshComponent* GetNerveBall() const {return NerveBall;}

	UPROPERTY(EditDefaultsOnly, Category = "NerveBall")
	FRotator NerveBallRotationDelta = FRotator(0.0f, 90.0f, 0.0f);

	/**
	 * @brief Is attached to something else
	 * @return 
	 */
	bool IsNerveBallAttached() const;

#pragma endregion

#pragma region Interaction

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UENTInteractableComponent> InteractableComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> InteractionChannel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UAkAudioEvent> GrabNoise;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta=(ClampMin="-90.0", ClampMax="90.0", UIMin="-90", UIMax="90.0"))
	float EjectionAngleBuff = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta=(ClampMin="-90.0", ClampMax="90.0", UIMin="-90", UIMax="90.0"))
	bool bIsLigament = false;

	UPROPERTY()
	TObjectPtr<AENTDefaultPlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<AENTDefaultCharacter> PlayerCharacter;

	UPROPERTY()
	bool bIsInWeakZone = false;

public:
	TObjectPtr<UENTInteractableComponent> GetInteractable() const {return InteractableComponent;}

	float GetEjectionAngleBuff() const {return EjectionAngleBuff;}

	UFUNCTION()
	void Interaction(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent);

#pragma endregion

#pragma region Physics

protected:
	UPROPERTY()
	TObjectPtr<UENTPhysicConstraint> PhysicConstraint;

	UPROPERTY(EditAnywhere, Category = "Physics", meta = (ClampMin = 0.0f, Units = "cm"))
	float DistanceNeededToPropulsion = 500.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Physics")
	bool bIsStretchSoundPlayed = false;

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

	FTimeline EnterWeakZoneTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Weak Zone")
	TObjectPtr<UCurveFloat> EnterWeakZoneCurve;

	UPROPERTY()
	UMaterialInstanceDynamic* CorruptMID;

	UFUNCTION()
	void UpdateEnterWeakZone(float Alpha);

	
	UPROPERTY(EditDefaultsOnly, Category = "Weak Zone")
	float RetractLigamentDuration = 1.5f;

	UPROPERTY()
	float StartZ;

#pragma endregion

#pragma region Save

protected:
	bool bIsLoaded = false;

public:
	virtual FENTGameElementData& SaveGameElement(UENTWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave) override;

	bool IsLoaded() const {return bIsLoaded;}

#pragma endregion

protected:
	UPROPERTY()
	TObjectPtr<AENTNerveReceptacle> CurrentAttachedReceptacle;

public:
	void SetCurrentReceptacle(AENTNerveReceptacle* Receptacle);

#if WITH_EDITORONLY_DATA
protected:
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDebugNerve = false;

#endif
};
