// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CharacterStateSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class PROTOPROFONDEURS_API UCharacterStateSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Ground")
	TEnumAsByte<EPhysicalSurface> SlipperySurface = SurfaceType1;
};
