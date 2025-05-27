// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/NerveReactive.h"
#include "NerveDoor.generated.h"

class UAkComponent;

UCLASS()
class PROTOPROFONDEURS_API ANerveDoor : public AActor, public INerveReactive
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANerveDoor();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> MeshDoor;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DoorMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkComponent> NerveDoorNoises;

	UPROPERTY(EditAnywhere)
	bool IsActiveAtStart;

	UPROPERTY()
	bool IsOpened;

	UPROPERTY()
	bool IsLocked;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Trigger_Implementation() override;
	virtual void SetLock_Implementation(bool state) override;
	bool GetActiveAtStart() const { return IsActiveAtStart; }
};
