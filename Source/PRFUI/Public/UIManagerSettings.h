// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Menus/Main/PRFCreditsMenu.h"
#include "Menus/Main/PRFMainMenu.h"
#include "Menus/Main/PRFNewGameMenu.h"
#include "Menus/Main/PRFPressAny.h"
#include "Menus/Main/PRFQuitMenu.h"
#include "Menus/Options/PRFControlsMenu.h"
#include "Menus/Pause/PRFMainMenuConfirmationMenu.h"
#include "Menus/Pause/PRFOptionsMenu.h"
#include "Menus/Pause/PRFPauseMenu.h"
#include "UIManagerSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class PRFUI_API UUIManagerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

#pragma region Main Menus

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI")
	TSubclassOf<UPRFPressAny> PressAnyMenuClass = UPRFPauseMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI")
	TSubclassOf<UPRFMainMenu> MainMenuClass = UPRFPauseMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI")
	TSubclassOf<UPRFNewGameMenu> NewGameMenuClass = UPRFNewGameMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI")
	TSubclassOf<UPRFCreditsMenu> CreditsMenuClass = UPRFCreditsMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI")
	TSubclassOf<UPRFQuitMenu> QuitMenuClass = UPRFQuitMenu::StaticClass();

#pragma endregion

#pragma region Options Menus
	
	UPROPERTY(EditDefaultsOnly, Config, Category = "UI")
	TSubclassOf<UPRFPauseMenu> PauseMenuClass = UPRFPauseMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI")
	TSubclassOf<UPRFOptionsMenu> OptionsMenuClass = UPRFOptionsMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI")
	TSubclassOf<UPRFControlsMenu> ControlsMenuClass = UPRFControlsMenu::StaticClass();

	UPROPERTY(EditDefaultsOnly, Config, Category = "UI")
	TSubclassOf<UPRFMainMenuConfirmationMenu> MainMenuConfirmationMenuClass = UPRFMainMenuConfirmationMenu::StaticClass();

#pragma endregion
};
