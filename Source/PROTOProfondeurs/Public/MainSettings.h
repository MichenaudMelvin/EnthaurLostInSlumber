// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MainSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "Main Settings"))
class PROTOPROFONDEURS_API UMainSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Nerve")
	FVector PawnGrabObjectOffset;
};
