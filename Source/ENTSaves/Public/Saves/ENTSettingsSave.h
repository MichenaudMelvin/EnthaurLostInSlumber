// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTDefaultSave.h"
#include "ENTSettingsSave.generated.h"

UCLASS()
class ENTSAVES_API UENTSettingsSave : public UENTDefaultSave
{
	GENERATED_BODY()

public:
	UENTSettingsSave();

#pragma region Accessibility

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Accessibility")
	bool bInvertYAxis = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Accessibility")
	bool bViewBobbing = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Accessibility", meta = (ClampMin = 0.1f, ClampMax = 10.0f, UIMin = 0.1f, UIMax = 10.0f))
	float MouseSensitivity = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds", meta = (ClampMin = 0.0f, ClampMax = 100.0f, UIMin = 0.0f, UIMax = 100.0f))
	float MasterVolume = 80.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds", meta = (ClampMin = 0.0f, ClampMax = 100.0f, UIMin = 0.0f, UIMax = 100.0f))
	float SFXVolume = 80.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds", meta = (ClampMin = 0.0f, ClampMax = 100.0f, UIMin = 0.0f, UIMax = 100.0f))
	float MusicVolume = 80.0f;

#pragma endregion
};
