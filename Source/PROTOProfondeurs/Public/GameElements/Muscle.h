// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeakZoneInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Interface/GroundAction.h"
#include "Interface/NerveReactive.h"
#include "Saves/WorldSaves/SaveGameElementInterface.h"

#if WITH_EDITORONLY_DATA
#include "Player/FirstPersonCharacter.h"
#include "Player/States/CharacterFallState.h"
#endif

#include "Muscle.generated.h"

USTRUCT(BlueprintType)
struct FMuscleData : public FGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool bIsSolid = false;
};

class UAkAudioEvent;
class UInteractableComponent;

UCLASS()
class PROTOPROFONDEURS_API AMuscle : public AActor, public IGroundAction, public INerveReactive, public IWeakZoneInterface, public ISaveGameElementInterface
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	FVector2D MuscleSize = FVector2D(5.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
	FFloatRange MuscleHeight = FFloatRange(5.0f, 10.0f);

	UPROPERTY(BlueprintReadOnly, Category = "Dimensions")
	float CurrentZScale = 0.0f;

	float MuscleZUnit = 0.0f;

#pragma endregion

#pragma region Deformation

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Deformation")
	bool bIsSolid = true;

	bool bDefaultSolidity = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deformation", meta = (Units = "s"))
	float DeformationSpeed = 0.5f;

	float DeformationAlpha = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deformation", meta = (Units = "s"))
	float DeformationDuration = 1.0f;

	int8 DeformationDirection = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deformation")
	float MinPossibleScale = 0.01f;

	bool bTriggerDeformation = false;

	UPROPERTY(EditDefaultsOnly, Category = "Deformation")
	TObjectPtr<UCurveFloat> DeformationCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Deformation")
	TObjectPtr<UAkAudioEvent> DeformationNoise;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics|Debug")
	TObjectPtr<class UBoxComponent> TraceExtentVisibility;

	UPROPERTY(BlueprintReadOnly, Category = "Physics|Debug")
	TObjectPtr<class UArrowComponent> BounceDirectionTop;

	UPROPERTY(BlueprintReadOnly, Category = "Physics|Debug")
	TObjectPtr<class UArrowComponent> BounceDirectionBack;
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics")
	FVector TraceExtent = FVector(-300.0f, 0.0f, 50.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Physics")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsToCheck;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics")
	float TraceHeight = 50.0f;

	UPROPERTY()
	bool bUseFixedVelocity = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Physics", meta = (ClampMin = 0.0f, ClampMax = 10.0f, UIMin = 0.0f, UIMax = 10.0f, EditCondition = "!bUseFixedVelocity"))
	float VelocityMultiplier = 1.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Physics", meta = (ClampMin = 0.0f, EditCondition = bUseFixedVelocity))
	float FixedVelocity = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics", meta = (ClampMin = 0.0f, ClampMax = 2500.0f, UIMin = 0.0f, UIMax = 2500.0f))
	float MinTriggerVelocity = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics", meta = (ClampMin = 0.0f, UIMin = 0.0f))
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance", meta = (Units = "s"))
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

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TObjectPtr<UAkAudioEvent> InteractionEvent;

	UFUNCTION()
	void Interact(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractComponent);

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

#pragma region Save

public:
	virtual void SaveGameElement(UWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FGameElementData& GameElementData) override;

#pragma endregion

#pragma region Debug

#if WITH_EDITORONLY_DATA
protected:
	void OnSelectionUpdate(UObject* Object);

	void DrawProjections();

	void ClearProjectionDraw() const;

	bool SelectedInEditor = false;

	void DrawSingleProjection(const struct FPredictProjectilePathParams& Params, const FVector& StartPoint, const FVector& Velocity, const FColor& DrawColor) const;

	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDrawInRunTime = false;

	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDrawHitCapsule = false;

	UPROPERTY(EditInstanceOnly, Category = "Debug")
	float MaxSimTime = 2.0f;

	UPROPERTY(EditInstanceOnly, Category = "Debug")
	float SimFrequency = 5.0f;

	UPROPERTY(VisibleInstanceOnly, Category = "Debug")
	FColor MinVelocityColor = FColor::Green;

	UPROPERTY(VisibleInstanceOnly, Category = "Debug")
	FColor MaxVelocityColor = FColor::Blue;

	UPROPERTY(VisibleInstanceOnly, Category = "Debug")
	FColor HitColor = FColor::Red;

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	TSubclassOf<AFirstPersonCharacter> FirstPersonCharacterClass = AFirstPersonCharacter::StaticClass();

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	TSubclassOf<UCharacterFallState> CharacterFallStateClass = UCharacterFallState::StaticClass();
#endif

#pragma endregion
};
