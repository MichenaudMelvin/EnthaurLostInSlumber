// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DefaultInputSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "InputSettings"))
class PROTOPROFONDEURS_API UDefaultInputSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditDefaultsOnly, Category = "InputSettings", meta = (UIMin = 0.0f, UIMax = 1.0f, ClampMin = 0.0f, ClampMax = 1.0f))
	float MoveInputThreshold = 0.1f;  
};
