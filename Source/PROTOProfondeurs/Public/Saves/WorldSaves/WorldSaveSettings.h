// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "WorldSaveSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class PROTOPROFONDEURS_API UWorldSaveSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/**
	 * @brief The very first gameplay of the game
	 */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "World")
	TSoftObjectPtr<UWorld> StartGameplayWorld;

	/**
	 * @brief Allowed GameModes for world saves
	 */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "WorldSave")
	TSet<TSubclassOf<AGameModeBase>> AllowedGameModes;
};
