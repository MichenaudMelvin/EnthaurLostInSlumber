// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// #if !UE_BUILD_SHIPPING

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FirstPersonSpectator.generated.h"

class UCameraComponent;
class AFirstPersonController;
class UFloatingPawnMovement;

UCLASS()
class PROTOPROFONDEURS_API AFirstPersonSpectator : public APawn
{
	GENERATED_BODY()

public:
	AFirstPersonSpectator();

protected:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "Spectator")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, Category = "Spectator")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY()
	TObjectPtr<AFirstPersonController> FirstPersonController;

	UPROPERTY(EditDefaultsOnly, Category = "Spectator")
	TObjectPtr<UFloatingPawnMovement> Movement;

	UPROPERTY(EditDefaultsOnly, Category = "Spectator", meta = (ClampMin = 0.1f))
	float CameraSpeed = 0.75f;

	UFUNCTION()
	void RotatePitch(float AxisValue);

	UFUNCTION()
	void RotateYaw(float AxisValue);
};
// #endif
