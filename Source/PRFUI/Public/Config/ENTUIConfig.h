// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HUD/ENTDeathTransition.h"
#include "HUD/ENTGameplayHUD.h"
#include "HUD/ENTLevelEntering.h"
#include "Menus/Main/PRFCreditsMenu.h"
#include "Menus/Main/PRFMainMenu.h"
#include "Menus/Main/PRFNewGameMenu.h"
#include "Menus/Main/PRFPressAny.h"
#include "Menus/Main/PRFQuitMenu.h"
#include "Menus/Options/PRFControlsMenu.h"
#include "Menus/Pause/PRFMainMenuConfirmationMenu.h"
#include "Menus/Pause/PRFOptionsMenu.h"
#include "Menus/Pause/PRFPauseMenu.h"
#include "Menus/Pause/PRFRestartConfirmationMenu.h"
#include "ENTUIConfig.generated.h"

USTRUCT(Blueprintable)
struct FDuoText
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "UI")
	FText AreaZone;

	UPROPERTY(EditAnywhere, Category = "UI")
	FText RegionZone;
};

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "UIConfig"))
class PRFUI_API UENTUIConfig : public UDeveloperSettings
{
	GENERATED_BODY()

#pragma region Main Menus
public:
	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UPRFPressAny> PressAnyMenuClass = UPRFPauseMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UPRFMainMenu> MainMenuClass = UPRFPauseMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UPRFNewGameMenu> NewGameMenuClass = UPRFNewGameMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UPRFCreditsMenu> CreditsMenuClass = UPRFCreditsMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UPRFQuitMenu> QuitMenuClass = UPRFQuitMenu::StaticClass();

#pragma endregion

#pragma region Options Menus

public:
	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UPRFPauseMenu> PauseMenuClass = UPRFPauseMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UPRFOptionsMenu> OptionsMenuClass = UPRFOptionsMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UPRFControlsMenu> ControlsMenuClass = UPRFControlsMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UPRFMainMenuConfirmationMenu> MainMenuConfirmationMenuClass = UPRFMainMenuConfirmationMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|Widgets")
	TSubclassOf<UPRFRestartConfirmationMenu> RestartConfirmationMenuClass = UPRFRestartConfirmationMenu::StaticClass();

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
	TMap<TSoftObjectPtr<UWorld>, FDuoText> LevelNames;

	/**
	 * @brief Set of GameModes where HUD will be used
	 */
	UPROPERTY(EditDefaultsOnly, Config, Category = "UI|HUD")
	TSet<TSubclassOf<AGameModeBase>> AllowedHUDGameModes;

#pragma endregion

};
