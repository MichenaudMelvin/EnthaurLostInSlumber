// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SaveSettings.generated.h"

class UAkRtpc;

UCLASS(Config = Game, DefaultConfig, DisplayName = "SettingsSave")
class PROTOPROFONDEURS_API USaveSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TSoftObjectPtr<UAkRtpc> MasterRTPC;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TSoftObjectPtr<UAkRtpc> SFXRTPC;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TSoftObjectPtr<UAkRtpc> MusicRTPC;
};
