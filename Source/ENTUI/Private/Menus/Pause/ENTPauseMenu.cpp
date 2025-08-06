// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Pause/ENTPauseMenu.h"
#include "Subsystems/ENTMenuManager.h"
#include "Config/ENTUIConfig.h"
#include "Subsystems/ENTLevelNameSubsystem.h"
#include "Components/TextBlock.h"

void UENTPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (OptionsMenuButton && OptionsMenuButton->GetCustomButton())
	{
		OptionsMenuButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTPauseMenu::HandleOptionsMenuButton);
	}
	if (RestartCheckpointButton && RestartCheckpointButton->GetCustomButton())
	{
		RestartCheckpointButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTPauseMenu::HandleRestartCheckpointButton);
	}
	if (MainMenuButton && MainMenuButton->GetCustomButton())
	{
		MainMenuButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTPauseMenu::HandleMainMenuButton);
	}
	if (QuitButton && QuitButton->GetCustomButton())
	{
		QuitButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTPauseMenu::HandleQuitButton);
	}

	UENTLevelNameSubsystem* LevelNameSubsystem = GetGameInstance()->GetSubsystem<UENTLevelNameSubsystem>();
	if (!IsValid(LevelNameSubsystem))
	{
		return;
	}

	AreaName->SetText(LevelNameSubsystem->GetZoneName().AreaName);
	RegionName->SetText(LevelNameSubsystem->GetZoneName().RegionName);
}

void UENTPauseMenu::NativeDestruct()
{
	Super::NativeDestruct();

	if (OptionsMenuButton && OptionsMenuButton->GetCustomButton())
	{
		OptionsMenuButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTPauseMenu::HandleOptionsMenuButton);
	}
	if (RestartCheckpointButton && RestartCheckpointButton->GetCustomButton())
	{
		RestartCheckpointButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTPauseMenu::HandleRestartCheckpointButton);
	}
	if (MainMenuButton && MainMenuButton->GetCustomButton())
	{
		MainMenuButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTPauseMenu::HandleMainMenuButton);
	}
	if (QuitButton && QuitButton->GetCustomButton())
	{
		QuitButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTPauseMenu::HandleQuitButton);
	}
}

void UENTPauseMenu::HandleMainMenuButton()
{
	UENTMenuManager* UIManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	const UENTUIConfig* UIManagerSettings = GetDefault<UENTUIConfig>();
	if (!IsValid(UIManagerSettings))
	{
		return;
	}

	UUserWidget* MainMenuConfirmationMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->MainMenuConfirmationMenuClass);
	UIManager->OpenMenu(MainMenuConfirmationMenu, false);
}

void UENTPauseMenu::HandleQuitButton()
{
	UENTMenuManager* UIManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	const UENTUIConfig* UIManagerSettings = GetDefault<UENTUIConfig>();
	if (!IsValid(UIManagerSettings))
	{
		return;
	}

	UUserWidget* QuitMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->QuitMenuClass);
	UIManager->OpenMenu(QuitMenu, false);
}

void UENTPauseMenu::HandleOptionsMenuButton()
{
	UENTMenuManager* UIManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	const UENTUIConfig* UIManagerSettings = GetDefault<UENTUIConfig>();
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

void UENTPauseMenu::HandleRestartCheckpointButton()
{
	UENTMenuManager* UIManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	const UENTUIConfig* UIManagerSettings = GetDefault<UENTUIConfig>();
	if (!IsValid(UIManagerSettings))
	{
		return;
	}

	UUserWidget* RestartConfirmationMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->RestartConfirmationMenuClass);
	UIManager->OpenMenu(RestartConfirmationMenu, false);
}
