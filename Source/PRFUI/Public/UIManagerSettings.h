// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PauseMenus/PRFPauseMenu.h"
#include "UIManagerSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class PRFUI_API UUIManagerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Config, Category = "UI")
	TSubclassOf<UPRFPauseMenu> PauseMenuClass = UPRFPauseMenu::StaticClass();
};
