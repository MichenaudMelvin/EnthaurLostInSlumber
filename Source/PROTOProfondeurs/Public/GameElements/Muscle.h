// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeakZoneInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Interface/GroundAction.h"
#include "Interface/NerveReactive.h"
#include "Muscle.generated.h"

class UInteractableComponent;

UCLASS()
class PROTOPROFONDEURS_API AMuscle : public AActor, public IGroundAction, public INerveReactive, public IWeakZoneInterface
{
	GENERATED_BODY()

#pragma region Defaults

public:
	AMuscle();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Componoents")
	TObjectPtr<USceneComponent> Root;

#pragma endregion

#pragma region Dimensions

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MuscleMeshComp;

	UPROPERTY(EditAnywhere, Category = "Dimensions")
	FVector2D MuscleSize = FVector2D(5.0f);

	UPROPERTY(EditAnywhere, Category = "Dimensions")
	FFloatRange MuscleHeight = FFloatRange(5.0f, 10.0f);

	float CurrentZScale = 0.0f;

	float MuscleZUnit = 0.0f;

#pragma endregion

#pragma region Deformation

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Deformation")
	bool bIsSolid = true;

	bool bDefaultSolidity = true;

	UPROPERTY(EditDefaultsOnly, Category = "Deformation", meta = (Units = "s"))
	float DeformationSpeed = 0.5f;

	float DeformationAlpha = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Deformation", meta = (Units = "s"))
	float DeformationDuration = 1.0f;

	int8 DeformationDirection = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Deformation")
	float MinPossibleScale = 0.01f;

	bool bTriggerDeformation = false;

	UPROPERTY(EditDefaultsOnly, Category = "Deformation")
	TObjectPtr<UCurveFloat> DeformationCurve;

	UFUNCTION(CallInEditor, Category = "Deformation")
	void StartDeformation();

	void EndDeformation();

	void DeformMesh(float DeltaTime);

	bool IsDeformed() const;

	void RebuildMuscleMesh() const;

	void UpdateMuscleSolidity();

#pragma endregion

#pragma region Physics

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<class UBoxComponent> TraceExtentVisibility;

	UPROPERTY()
	TObjectPtr<class UArrowComponent> BounceDirectionTop;

	UPROPERTY()
	TObjectPtr<class UArrowComponent> BounceDirectionBack;
#endif

	UPROPERTY(EditDefaultsOnly, Category = "Physics")
	FVector TraceExtent = FVector(-300.0f, 0.0f, 50.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Physics")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsToCheck;

	UPROPERTY(EditDefaultsOnly, Category = "Physics")
	float TraceHeight = 50.0f;

	UPROPERTY()
	bool bUseFixedVelocity = false;

	UPROPERTY(EditInstanceOnly, Category = "Physics", meta = (ClampMin = 0.0f, ClampMax = 10.0f, UIMin = 0.0f, UIMax = 10.0f, EditCondition = "!bUseFixedVelocity"))
	float VelocityMultiplier = 1.0f;

	UPROPERTY(EditInstanceOnly, Category = "Physics", meta = (ClampMin = 0.0f, EditCondition = bUseFixedVelocity))
	float FixedVelocity = 5000.0f;

	UPROPERTY(EditAnywhere, Category = "Physics", meta = (ClampMin = 0.0f, ClampMax = 2500.0f, UIMin = 0.0f, UIMax = 2500.0f))
	float MinTriggerVelocity = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Physics", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxLaunchVelocity = 3500.0f;

	UFUNCTION(CallInEditor, Category = "Physics")
	void ToggleMuscleSolidity();

	UFUNCTION()
	void HitMuscleMesh(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void HitMuscle(AActor* HitActor, UPrimitiveComponent* OtherComp);

	bool IsActorInRange(AActor* Actor);

#pragma endregion

#pragma region Appearance

protected:
	UPROPERTY(EditAnywhere, Category = "Appearance")
	TObjectPtr<UStaticMesh> MuscleMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	TObjectPtr<UMaterialInterface> MuscleMaterial;

	FTimeline MuscleStateTransitionTimeline;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	TObjectPtr<UCurveFloat> MuscleStateTransitionCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	FName MuscleStateTransitionParam = FName("MuscleStateTransition");

	UPROPERTY(EditDefaultsOnly, Category = "Appearance", meta = (Units = "s"))
	float MuscleStateTransitionDuration = 0.5f;

	UFUNCTION()
	void UpdateMuscleStateTransition(float Alpha);

#pragma endregion

#pragma region Interaction

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> SpikeInteraction;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TObjectPtr<UInteractableComponent> Interactable;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FText SolidMuscleInteraction;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FText SoftMuscleInteraction;

	UPROPERTY(EditInstanceOnly, Category = "Interaction")
	bool bAllowInteraction = true;

	UFUNCTION()
	void Interact(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractComponent);

#pragma endregion

#pragma region Debug

#if WITH_EDITORONLY_DATA

protected:
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDebug = false;

#endif

#pragma endregion

#pragma region Interfaces

protected:
	virtual void OnActorAbove_Implementation(AActor* Actor) override;

	virtual void OnEnterWeakZone_Implementation(bool bIsZoneActive) override;

	virtual void OnExitWeakZone_Implementation() override;

	bool bIsLocked = false;

	virtual void Trigger_Implementation() override;

	virtual void SetLock_Implementation(bool state) override;

#pragma endregion
};
