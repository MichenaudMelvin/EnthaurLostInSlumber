// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HUD/ENTDeathTransition.h"
#include "HUD/ENTGameplayHUD.h"
#include "HUD/ENTLevelEntering.h"
#include "Menus/Main/ENTCreditsMenu.h"
#include "Menus/Main/ENTMainMenu.h"
#include "Menus/Main/ENTNewGameMenu.h"
#include "Menus/Main/ENTPressAny.h"
#include "Menus/Main/ENTQuitMenu.h"
#include "Menus/Options/ENTControlsMenu.h"
#include "Menus/Pause/ENTMainMenuConfirmationMenu.h"
#include "Menus/Pause/ENTOptionsMenu.h"
#include "Menus/Pause/ENTPauseMenu.h"
#include "Menus/Pause/ENTRestartConfirmationMenu.h"
#include "ENTUIConfig.generated.h"

USTRUCT(Blueprintable, Category = "UI")
struct FENTZoneName
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FText AreaName;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FText RegionName;
};

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "UI Config"))
class ENTUI_API UENTUIConfig : public UDeveloperSettings
{
	GENERATED_BODY()

#pragma region Main Menus
public:
	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UENTPressAny> PressAnyMenuClass = UENTPauseMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UENTMainMenu> MainMenuClass = UENTPauseMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UENTNewGameMenu> NewGameMenuClass = UENTNewGameMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UENTCreditsMenu> CreditsMenuClass = UENTCreditsMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UENTQuitMenu> QuitMenuClass = UENTQuitMenu::StaticClass();

#pragma endregion

#pragma region Options Menus

public:
	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UENTPauseMenu> PauseMenuClass = UENTPauseMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UENTOptionsMenu> OptionsMenuClass = UENTOptionsMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UENTControlsMenu> ControlsMenuClass = UENTControlsMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UENTMainMenuConfirmationMenu> MainMenuConfirmationMenuClass = UENTMainMenuConfirmationMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UENTRestartConfirmationMenu> RestartConfirmationMenuClass = UENTRestartConfirmationMenu::StaticClass();

#pragma endregion

#pragma region HUD

public:
	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|HUD")
	TSubclassOf<UENTGameplayHUD> GameplayHUDClass = UENTGameplayHUD::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|HUD")
	TSubclassOf<UENTDeathTransition> DeathTransitionClass = UENTDeathTransition::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|HUD")
	TSubclassOf<UENTLevelEntering> LevelEnteringClass = UENTLevelEntering::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|HUD")
	TMap<TSoftObjectPtr<UWorld>, FENTZoneName> ZoneNames;

	/**
	 * @brief Set of GameModes where HUD will be used
	 */
	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|HUD")
	TSet<TSubclassOf<AGameModeBase>> AllowedHUDGameModes;

#pragma endregion

};
