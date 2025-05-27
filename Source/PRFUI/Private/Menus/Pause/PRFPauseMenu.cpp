// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Pause/PRFPauseMenu.h"
#include "PRFUIManager.h"
#include "UIManagerSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/LevelNameSubsystem.h"

void UPRFPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (OptionsMenuButton)
	{
		OptionsMenuButton->OnClicked.AddDynamic(this, &UPRFPauseMenu::HandleOptionsMenuButton);
	}
	if (RestartCheckpointButton)
	{
		RestartCheckpointButton->OnClicked.AddDynamic(this, &UPRFPauseMenu::HandleRestartCheckpointButton);
	}
	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UPRFPauseMenu::HandleMainMenuButton);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UPRFPauseMenu::HandleQuitButton);
	}

	
	ULevelNameSubsystem* LevelNameSubsystem = GetGameInstance()->GetSubsystem<ULevelNameSubsystem>();
	if (!IsValid(LevelNameSubsystem))
	{
		return;
	}

	AreaName->SetText(LevelNameSubsystem->GetLevelName().AreaZone);
	RegionName->SetText(LevelNameSubsystem->GetLevelName().RegionZone);
}

void UPRFPauseMenu::NativeDestruct()
{
	Super::NativeDestruct();

	if (OptionsMenuButton)
	{
		OptionsMenuButton->OnClicked.RemoveDynamic(this, &UPRFPauseMenu::HandleOptionsMenuButton);
	}
	if (RestartCheckpointButton)
	{
		RestartCheckpointButton->OnClicked.RemoveDynamic(this, &UPRFPauseMenu::HandleRestartCheckpointButton);
	}
	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.RemoveDynamic(this, &UPRFPauseMenu::HandleMainMenuButton);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.RemoveDynamic(this, &UPRFPauseMenu::HandleQuitButton);
	}
}

void UPRFPauseMenu::HandleMainMenuButton()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	UIManager->CloseAllMenus(EPRFUIState::AnyMenu);
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainMenuLevel);
}

void UPRFPauseMenu::HandleQuitButton()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	const UUIManagerSettings* UIManagerSettings = GetDefault<UUIManagerSettings>();
	if (!IsValid(UIManagerSettings))
	{
		return;
	}

	UUserWidget* QuitMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->QuitMenuClass);
	UIManager->OpenMenu(QuitMenu, false);
}

void UPRFPauseMenu::HandleOptionsMenuButton()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	const UUIManagerSettings* UIManagerSettings = GetDefault<UUIManagerSettings>();
	if (!IsValid(UIManagerSettings))
	{
		return;
	}

	if (!IsValid(UIManagerSettings->OptionsMenuClass))
	{
		return;
	}

	UUserWidget* OptionsMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->OptionsMenuClass);
	UIManager->OpenMenu(OptionsMenu, false);
}

void UPRFPauseMenu::HandleRestartCheckpointButton()
{
	// Todo Melvin
}
