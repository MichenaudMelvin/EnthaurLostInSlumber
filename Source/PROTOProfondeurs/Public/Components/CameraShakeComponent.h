// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraShakeComponent.generated.h"


class AFirstPersonCharacter;
class AFirstPersonController;

UCLASS(ClassGroup = ("Player"), meta = (BlueprintSpawnableComponent))
class PROTOPROFONDEURS_API UCameraShakeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCameraShakeComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shake")
	TSubclassOf<UCameraShakeBase> LittleShake;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shake")
	TSubclassOf<UCameraShakeBase> BigShake;

	UPROPERTY()
	AFirstPersonController* PlayerController;

public:
	UFUNCTION(BlueprintCallable, Category = "Shake")
	void MakeSmallCameraShake();

	UFUNCTION(BlueprintCallable, Category = "Shake")
	void MakeBigCameraShake();
};
