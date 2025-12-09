// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ENTSavesConfig.generated.h"

class UAkRtpc;

UCLASS(Config = Game, DefaultConfig, DisplayName = "Saves Config")
class ENTSAVES_API UENTSavesConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/**
	 * @brief The very first gameplay of the game
	 */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "WorldSave")
	TSoftObjectPtr<UWorld> StartGameplayWorld;

	/**
	 * @brief Allowed GameModes for world saves
	 */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "WorldSave")
	TSet<TSubclassOf<AGameModeBase>> AllowedGameModes;

	/**
	 * @brief Map assets
	 */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "WorldSave")
	FPrimaryAssetType PrimaryMapAsset;

	/**
	 * @brief Master volume RTPC
	 */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Audio")
	TSoftObjectPtr<UAkRtpc> MasterRTPC;

	/**
	 * @brief SFX Volume RTPC
	 */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Audio")
	TSoftObjectPtr<UAkRtpc> SFXRTPC;

	/**
	 * @brief Music volume RTPC
	 */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Audio")
	TSoftObjectPtr<UAkRtpc> MusicRTPC;
};
