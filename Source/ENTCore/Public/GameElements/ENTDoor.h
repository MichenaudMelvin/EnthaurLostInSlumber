// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	bool bIsOpened;

	bool bIsLocked;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void Trigger_Implementation() override;

	virtual void SetLock_Implementation(bool bState) override;

	bool GetActiveAtStart() const { return IsActiveAtStart; }
};
