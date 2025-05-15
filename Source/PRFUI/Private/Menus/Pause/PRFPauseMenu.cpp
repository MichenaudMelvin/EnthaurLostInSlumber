// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Pause/PRFPauseMenu.h"
#include "PRFUIManager.h"
#include "UIManagerSettings.h"
#include "Kismet/GameplayStatics.h"

void UPRFPauseMenu::TogglePauseMenu()
{
	
}

void UPRFPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UPRFPauseMenu::HandleMainMenuButton);
	}
	if (OptionsMenuButton)
	{
		OptionsMenuButton->OnClicked.AddDynamic(this, &UPRFPauseMenu::HandleOptionsMenuButton);
	}
}

void UPRFPauseMenu::NativeDestruct()
{
	Super::NativeDestruct();

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.RemoveDynamic(this, &UPRFPauseMenu::HandleMainMenuButton);
	}
	if (OptionsMenuButton)
	{
		OptionsMenuButton->OnClicked.RemoveDynamic(this, &UPRFPauseMenu::HandleOptionsMenuButton);
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

	//UIManager->SetMenuState(EPRFUIState::AnyMenu);
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainMenuLevel);
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