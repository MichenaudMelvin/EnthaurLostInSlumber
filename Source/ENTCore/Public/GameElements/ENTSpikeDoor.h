// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Interface/ENTActivation.h"
#include "ENTSpikeDoor.generated.h"

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
	
	UPROPERTY(EditDefaultsOnly, Category="Door")
	TObjectPtr<UStaticMeshComponent> LeftFrame;

	UPROPERTY(EditDefaultsOnly, Category="Door")
	TObjectPtr<UStaticMeshComponent> RightFrame;
	
	UPROPERTY(EditDefaultsOnly, Category="Door")
	TObjectPtr<UStaticMesh> InterMeshA;

	UPROPERTY(EditDefaultsOnly, Category="Door")
	TObjectPtr<UStaticMesh> InterMeshB;

	UPROPERTY(EditAnywhere, Category="Door", meta = (Units = cm))
	float DoorWidth = 200.f;

	UPROPERTY(EditDefaultsOnly, Category="Door")
	float SpacingFactor = 0.5f;

	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> InterMeshes;

	UPROPERTY()
	TArray<FVector> InterInitialRelativeLocations;

	UPROPERTY()
	TArray<float> InterStartOffsets;

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

public:
	virtual void Tick(float DeltaTime) override;

	virtual void Trigger_Implementation() override;

	virtual void SetLock_Implementation(bool bState) override;
};
