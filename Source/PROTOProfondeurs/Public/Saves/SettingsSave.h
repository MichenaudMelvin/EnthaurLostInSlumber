// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultSave.h"
#include "SettingsSave.generated.h"

UCLASS()
class PROTOPROFONDEURS_API USettingsSave : public UDefaultSave
{
	GENERATED_BODY()

public:
	USettingsSave();

#pragma region Accessibility

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Accessibility")
	bool bInvertYAxis = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Accessibility")
	bool bViewBobbing = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Accessibility")
	bool bCameraSteering = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Accessibility", meta = (ClampMin = 0.1f, ClampMax = 10.0f, UIMin = 0.1f, UIMax = 10.0f))
	float MouseSensitivity = 1.0f;

#pragma endregion
};
