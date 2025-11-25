// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Interface/ENTActivation.h"
#include "ENTSpikeDoor.generated.h"

class UBoxComponent;
class UAkComponent;

UCLASS()
class ENTCORE_API AENTSpikeDoor : public AActor, public IENTActivation
{
	GENERATED_BODY()

public:
	AENTSpikeDoor();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditDefaultsOnly, Category="Door")
	TObjectPtr<USceneComponent> Root;

	/**
	 * @brief To make this component works please check that the runtime generation of the navMesh is set to DynamicOnly
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Door|Navigation")
	TObjectPtr<UBoxComponent> DoorNavModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Door|Navigation", meta = (Units = cm, ClampMin = 0.0f))
	float NavModifierHeightOffset = 100.0f;

	UPROPERTY(VisibleInstanceOnly, Category = "Door|Navigation")
	FVector NavModifierDefaultLocation;

	UPROPERTY(VisibleInstanceOnly, Category = "Door|Navigation")
	FVector NavModifierOpenedLocation;

	UPROPERTY(EditDefaultsOnly, Category="Door")
	TObjectPtr<UStaticMeshComponent> LeftFrame;

	UPROPERTY(EditDefaultsOnly, Category="Door")
	TObjectPtr<UStaticMeshComponent> RightFrame;

	UPROPERTY(EditAnywhere, Category="Door", meta = (Units = cm))
	float DoorWidth = 200.f;

	UPROPERTY(EditDefaultsOnly, Category="Door")
	float SpacingFactor = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Door")
	TObjectPtr<UInstancedStaticMeshComponent> InterMeshesA;

	UPROPERTY(EditDefaultsOnly, Category = "Door")
	TObjectPtr<UInstancedStaticMeshComponent> InterMeshesB;

	UPROPERTY()
	TArray<FVector> InterInitialRelativeLocations;

	UPROPERTY()
	TArray<float> InterStartOffsets;

	UPROPERTY()
	TArray<bool> IsInterMeshA;

	UPROPERTY()
	TArray<FRotator> InterInitialRotations;

	UPROPERTY(EditDefaultsOnly, Category="Door")
	TObjectPtr<UCurveFloat> DropCurve;

	FTimeline DropTimeline;

	UPROPERTY(EditInstanceOnly, Category="Door")
	FRandomStream RandomStream;

	UFUNCTION()
	void DropTimelineUpdate(float Alpha);

	UFUNCTION()
	void DropTimelineFinished();

	UPROPERTY(EditDefaultsOnly, Category = "Door", meta = (Units = "s"))
	float MaxStagger = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Door", meta = (Units = "s"))
	float OpenDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Door", meta = (Units = "s"))
	float CloseDuration = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkComponent> NerveDoorNoises;

	bool bIsOpened = false;

	UPROPERTY()
	float DropHeight;

	void GenerateInterMeshes();

	void ClearInterMeshes();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Door")
	void ToggleDoorState();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Door")
	void OpenDoor();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Door")
	void CloseDoor();

public:
	virtual void Tick(float DeltaTime) override;

	virtual void Trigger_Implementation() override;

	virtual void SetLock_Implementation(bool bState) override;

#if WITH_EDITORONLY_DATA
private:
	/**
	 * @brief Temp function, use it if the door insn't working correctly
	 */
	UFUNCTION(CallInEditor, Category = "Door")
	void ClearDoor();

#endif
};
