// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AkSwitchValue.h"
#include "Engine/DeveloperSettings.h"
#include "SurfaceSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class PROTOPROFONDEURS_API USurfaceSettings : public UDeveloperSettings
{
	GENERATED_BODY()

protected:
	UPROPERTY(Config, EditDefaultsOnly, Category = "Surface|Noise")
	TMap<TEnumAsByte<EPhysicalSurface>, TSoftObjectPtr<UAkSwitchValue>> SurfaceNoises;

public:
	UFUNCTION(BlueprintCallable, Category = "Surface|Noise")
	UAkSwitchValue* FindNoise(const EPhysicalSurface& Surface) const;
};
