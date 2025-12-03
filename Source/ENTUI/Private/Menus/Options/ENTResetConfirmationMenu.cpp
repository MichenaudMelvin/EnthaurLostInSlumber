// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Options/ENTResetConfirmationMenu.h"

#include "Menus/Elements/ENTCustomButton.h"
#include "Menus/Options/ENTControlsMenu.h"
#include "Saves/ENTSettingsSave.h"
#include "Subsystems/ENTMenuManager.h"
#include "Subsystems/ENTSettingsSaveSubsystem.h"

void UENTResetConfirmationMenu::SetMenuType(EENTResetMenuType InMenuType)
{
	MenuType = InMenuType;
}

void UENTResetConfirmationMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTResetConfirmationMenu::HandleResetAction);
	}
}

void UENTResetConfirmationMenu::NativeConstruct()
{
	Super::NativeConstruct();

	switch (MenuType)
	{
	case EENTResetMenuType::Options:
		ResetText->SetText(NSLOCTEXT("UI", "ResetText", "Are you sure you want to\n reset all of your settings?"));
		break;
	case EENTResetMenuType::Volume:
		ResetText->SetText(NSLOCTEXT("UI", "ResetText", "Are you sure you want to\n reset the volume settings?"));
		break;
	case EENTResetMenuType::Accessibility:
		ResetText->SetText(NSLOCTEXT("UI", "ResetText", "Are you sure you want to\n reset the accessibility settings?"));
		break;
	case EENTResetMenuType::Brightness:
		ResetText->SetText(NSLOCTEXT("UI", "ResetText", "Are you sure you want to\n reset the brightness?"));
		break;
	case EENTResetMenuType::Controls:
		ResetText->SetText(NSLOCTEXT("UI", "ResetText", "Are you sure you want to\n reset the current control scheme?"));
		break;
	default:
		break;
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
	
	UENTSettingsSaveSubsystem* SettingsSaveSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!IsValid(SettingsSaveSubsystem))
	{
		return;
	}

	UENTSettingsSave* SettingsSave = SettingsSaveSubsystem->GetSettings();
	if (!IsValid(SettingsSave))
	{
		return;
	}

	UENTControlsMenu* ControlsMenu = Cast<UENTControlsMenu>(MenuManager->GetControlsMenu());
	if (!IsValid(ControlsMenu))
	{
		return;
	}

	if (!ResetText)
	{
		return;
	}

	switch (MenuType)
	{
	case EENTResetMenuType::Options:
		SettingsSubsystem->ResetSaveToDefault(0);
		ControlsMenu->ResetKeys();
		break;
	case EENTResetMenuType::Volume:
		SettingsSave->ResetVolumeSettings();
		break;
	case EENTResetMenuType::Accessibility:
		SettingsSave->ResetAccessibilitySettings();
		break;
	case EENTResetMenuType::Brightness:
		SettingsSave->ResetBrightnessSettings();
		break;
	case EENTResetMenuType::Controls:
		ControlsMenu->ResetKeys();
	break;
	default:
		break;
	}

	SettingsSaveSubsystem->SaveToSlot(0);
	MenuManager->CloseCurrentMenu();
}
