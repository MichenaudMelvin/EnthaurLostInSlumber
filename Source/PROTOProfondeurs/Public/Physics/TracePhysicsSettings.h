// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TracePhysicsSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class PROTOPROFONDEURS_API UTracePhysicsSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UTracePhysicsSettings();

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> InteractionTraceChannel = ECC_EngineTraceChannel1;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Ground")
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundObjectTypes;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Ground")
	TEnumAsByte<EPhysicalSurface> SlipperySurface = SurfaceType1;
};
