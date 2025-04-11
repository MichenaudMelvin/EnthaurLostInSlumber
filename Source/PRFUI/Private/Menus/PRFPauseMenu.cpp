// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/PRFPauseMenu.h"
#include "PRFUIManager.h"
#include "UIManagerSettings.h"

void UPRFPauseMenu::TogglePauseMenu()
{
	
}

void UPRFPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (OptionsMenuButton)
	{
		OptionsMenuButton->OnClicked.AddDynamic(this, &UPRFPauseMenu::HandleOptionsMenuButton);
	}
}

void UPRFPauseMenu::NativeDestruct()
{
	Super::NativeDestruct();

	if (OptionsMenuButton)
	{
		OptionsMenuButton->OnClicked.RemoveDynamic(this, &UPRFPauseMenu::HandleOptionsMenuButton);
	}
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
