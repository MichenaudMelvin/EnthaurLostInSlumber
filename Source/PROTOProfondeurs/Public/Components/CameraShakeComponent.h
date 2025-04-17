// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraShakeComponent.generated.h"


class AFirstPersonCharacter;
class AFirstPersonController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOPROFONDEURS_API UCameraShakeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCameraShakeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	TSubclassOf<UCameraShakeBase> WalkingWobbling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	TSubclassOf<UCameraShakeBase> RunningWobbling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shake")
	TSubclassOf<UCameraShakeBase> LittleShake;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void MakeSmallCameraShake();

private:
	UPROPERTY()
	AFirstPersonCharacter* Player;
	
	UPROPERTY()
	AFirstPersonController* PlayerController;

	UPROPERTY()
	bool b_IsRunning;
};
