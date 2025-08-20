// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Interface/ENTActivation.h"
#include "ENTDoor.generated.h"

class UAkComponent;

UCLASS()
class ENTCORE_API AENTDoor : public AActor, public IENTActivation
{
	GENERATED_BODY()

public:
	AENTDoor();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditDefaultsOnly, Category = "Door")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, Category = "Door")
	TObjectPtr<UStaticMeshComponent> MeshDoor;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DoorMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkComponent> NerveDoorNoises;

	UPROPERTY(EditAnywhere)
	bool IsActiveAtStart;

	bool bIsOpened;

	bool bIsLocked;

	UPROPERTY(EditDefaultsOnly, Category = "Door")
	FName DoorMaterialParam = "State";

	UPROPERTY(EditDefaultsOnly, Category = "Door", meta = (Units = s))
	float OpenDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Door", meta = (Units = s))
	float CloseDuration = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Door")
	TObjectPtr<UCurveFloat> DoorCurve;

	FTimeline DoorTimeline;

	UFUNCTION()
	void DoorTimelineUpdate(float Alpha);

public:
	virtual void Tick(float DeltaTime) override;

	virtual void Trigger_Implementation() override;

	virtual void SetLock_Implementation(bool bState) override;

	bool GetActiveAtStart() const { return IsActiveAtStart; }
};
