// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ENTSavedActor.h"
#include "Components/TimelineComponent.h"
#include "Interface/ENTActivation.h"
#include "ENTSpikeDoor.generated.h"

class UAkAudioEvent;
class UNiagaraComponent;
class UNiagaraSystem;
class UBoxComponent;
class UAkComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorOpened);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorClosed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTransitioningState, bool, bIsOpenning, float, Alpha);

UCLASS()
class ENTCORE_API AENTSpikeDoor : public AENTSavedActor, public IENTActivation
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

	UPROPERTY(EditDefaultsOnly, Category="Door")
	TObjectPtr<UNiagaraSystem> InterMeshFX;

	UPROPERTY(EditDefaultsOnly, Category="Door")
	float FX_X_Offset = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="Door")
	float FX_Z_Offset = 50.f;

	UPROPERTY(EditAnywhere, Category = "Door", meta = (Units = cm))
	float StartFXTraceHeight = 30.0f;

	UPROPERTY()
	TArray<TObjectPtr<UNiagaraComponent>> InterMeshFXComponents;

	UPROPERTY(BlueprintAssignable, Category = "Door")
	FOnDoorOpened OnDoorOpened;

	UPROPERTY(BlueprintAssignable, Category = "Door")
	FOnDoorClosed OnDoorClosed;

	UPROPERTY(BlueprintAssignable, Category = "Door")
	FOnTransitioningState OnTransitioningState;

	UPROPERTY()
	TArray<FVector> InterInitialRelativeLocations;

	UPROPERTY()
	TArray<float> InterStartOffsets;

	UPROPERTY()
	TArray<bool> IsInterMeshA;

	UPROPERTY()
	TArray<FRotator> InterInitialRotations;

	UPROPERTY()
	TArray<UInstancedStaticMeshComponent*> MeshesToUse;

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
	TObjectPtr<UAkAudioEvent> DoorOpenStartEvent;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkAudioEvent> DoorOpenEndEvent;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkAudioEvent> DoorCloseStartEvent;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkAudioEvent> DoorCloseEndEvent;

	UPROPERTY(BlueprintReadOnly, Category = "Door")
	bool bIsOpened = false;

	UPROPERTY()
	float DropHeight;

	void GenerateInterMeshes();

	void ClearInterMeshes();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Door")
	void ToggleDoorState();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Door")
	void OpenDoor() {OpenDoor(false);}

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Door")
	void CloseDoor() {CloseDoor(false);}

	void OpenDoor(bool bInstant);

	void CloseDoor(bool bInstant);

	bool bInstantEffect = false;

	void TriggerPlayerCameraShake() const;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void Trigger_Implementation() override;

	virtual void SetLock_Implementation(bool bState) override;

#pragma region Saves

protected:
	virtual FENTGameElementData& SaveGameElement(UENTWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave) override;

#pragma endregion

#if WITH_EDITOR
private:
	/**
	 * @brief Editor Only function, Use it if the door isn't working correctly
	 */
	UFUNCTION(CallInEditor, Category = "Door")
	void ClearDoor();
#endif
};
