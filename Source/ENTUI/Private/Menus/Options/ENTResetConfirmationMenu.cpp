// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Options/ENTResetConfirmationMenu.h"

#include "Menus/Elements/ENTCustomButton.h"
#include "Subsystems/ENTMenuManager.h"
#include "Subsystems/ENTSettingsSaveSubsystem.h"

void UENTResetConfirmationMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTResetConfirmationMenu::HandleResetAction);
	}
}

void UENTResetConfirmationMenu::BeginDestroy()
{
	Super::BeginDestroy();
	
	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTResetConfirmationMenu::HandleResetAction);
	}
}

void UENTResetConfirmationMenu::HandleResetAction()
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}

	SettingsSubsystem->ResetSaveToDefault(0);
	MenuManager->CloseCurrentMenu();
}
