// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/ViewBobbing.h"
#include "Engine/DeveloperSettings.h"
#include "CharacterSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class PROTOPROFONDEURS_API UCharacterSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UViewBobbing> ViewBobbingClass = UViewBobbing::StaticClass();

	UPROPERTY(Config, EditDefaultsOnly, Category = "Input", meta = (UIMin = 0.0f, UIMax = 1.0f, ClampMin = 0.0f, ClampMax = 1.0f))
	float MoveInputThreshold = 0.1f;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Nerve")
	FVector PawnGrabObjectOffset;
};
