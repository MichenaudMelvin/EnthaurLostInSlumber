// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DebugCameraController.h"
#include "ENTCustomDebugCamera.generated.h"

UCLASS()
class ENTDEVELOPERTOOLS_API AENTCustomDebugCamera : public ADebugCameraController
{
	GENERATED_BODY()

public:
	AENTCustomDebugCamera();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	bool Trace(FHitResult& HitResult) const;

#pragma region CustomActions

protected:
	void TeleportToFacingLocation();

	void DestroyFacingActor();

#pragma endregion

protected:
	/**
	 * @brief Default Trace is 5000.0f * 20.0f (100 000.0cm)
	 */
	float TraceLength = 100000.0f;

	/**
	 * @brief Cannot change it runtime, maybe later
	 */
	bool bResetCameraPitch = true;

#pragma region CameraSpeed

protected:
	/**
	 * @brief Cannot override the ADebugCameraController because the function isn't virtual
	 */
	void CustomIncreaseCameraSpeed();

	/**
	 * @brief Cannot override the ADebugCameraController because the function isn't virtual
	 */
	void CustomDecreaseCameraSpeed();

	FFloatRange CameraSpeed = FFloatRange(0.05f, 10.0f);

	/**
	 * @brief UE Debug Camera's value is 0.05f (SPEED_SCALE_ADJUSTMENT)
	 */
	float CameraSpeedScaleStep = 0.5f;

#pragma endregion
};
