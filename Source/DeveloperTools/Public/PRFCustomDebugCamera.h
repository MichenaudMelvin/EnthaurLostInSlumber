// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DebugCameraController.h"
#include "PRFCustomDebugCamera.generated.h"

UCLASS()
class DEVELOPERTOOLS_API APRFCustomDebugCamera : public ADebugCameraController
{
	GENERATED_BODY()

	virtual void SetupInputComponent() override;

	void Teleport();

	/**
	 * @brief Default Trace is 5000.0f * 20.0f (100 000.0cm)
	 */
	float TeleportTraceLength = 100000.0f;
};
