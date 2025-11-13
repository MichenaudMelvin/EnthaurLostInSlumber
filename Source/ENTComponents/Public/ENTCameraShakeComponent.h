// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ENTCameraShakeComponent.generated.h"


class AENTDefaultCharacter;
class AENTDefaultPlayerController;

UCLASS(ClassGroup = "Camera", meta = (BlueprintSpawnableComponent))
class ENTCOMPONENTS_API UENTCameraShakeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UENTCameraShakeComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shake")
	TSubclassOf<UCameraShakeBase> LittleShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shake")
	TSubclassOf<UCameraShakeBase> BigShake;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

public:
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Shake")
	void MakeSmallCameraShake() const;

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Shake")
	void MakeBigCameraShake() const;
};
