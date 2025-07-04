// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DebugCameraController.h"
#include "PRFCustomDebugCamera.generated.h"

UCLASS()
class DEVELOPERTOOLS_API APRFCustomDebugCamera : public ADebugCameraController
{
	GENERATED_BODY()

protected:
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
	float TeleportTraceLength = 100000.0f;
};
