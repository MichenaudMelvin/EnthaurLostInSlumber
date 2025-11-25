// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Pause/ENTOptionsMenu.h"
#include "Subsystems/ENTMenuManager.h"
#include "Config/ENTUIConfig.h"
#include "Components/Button.h"
#include "Kismet/KismetTextLibrary.h"
#include "Saves/ENTSettingsSave.h"
#include "Subsystems/ENTSettingsSaveSubsystem.h"
#include "Components/TextBlock.h"
#include "Menus/Elements/ENTCustomCheckBox.h"
#include "Menus/Elements/ENTCustomSlider.h"

void UENTOptionsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (VolumeButton && VolumeButton->GetCustomButton())
	{
		VolumeButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTOptionsMenu::OnVolumeButtonClicked);
	}
	if (ControlsButton && ControlsButton->GetCustomButton())
	{
		ControlsButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTOptionsMenu::OnViewControlsButtonClicked);
	}
	if (GammaButton && GammaButton->GetCustomButton())
	{
		GammaButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTOptionsMenu::OnGammaButtonClicked);
	}
	if (AccessibilityButton && AccessibilityButton->GetCustomButton())
	{
		AccessibilityButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTOptionsMenu::OnAccessibilityButtonClicked);
	}
}

void UENTOptionsMenu::NativeDestruct()
{
	Super::NativeDestruct();

	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->SaveToSlot(0);
}

void UENTOptionsMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (VolumeButton && VolumeButton->GetCustomButton())
	{
		VolumeButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTOptionsMenu::OnVolumeButtonClicked);
	}
	
	if (ControlsButton && ControlsButton->GetCustomButton())
	{
		ControlsButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTOptionsMenu::OnViewControlsButtonClicked);
	}
	if (GammaButton && GammaButton->GetCustomButton())
	{
		GammaButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTOptionsMenu::OnGammaButtonClicked);
	}
	if (AccessibilityButton && AccessibilityButton->GetCustomButton())
	{
		AccessibilityButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTOptionsMenu::OnAccessibilityButtonClicked);
	}
}

void UENTOptionsMenu::OnVolumeButtonClicked()
{
	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}
	
	MenuManager->OpenMenu(MenuManager->GetSoundMenu(), false);
}

void UENTOptionsMenu::OnViewControlsButtonClicked()
{
	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}

	MenuManager->OpenMenu(MenuManager->GetControlsMenu(), false);
}

void UENTOptionsMenu::OnGammaButtonClicked()
{
	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}
	
	MenuManager->OpenMenu(MenuManager->GetGammaMenu(), false);
}

void UENTOptionsMenu::OnAccessibilityButtonClicked()
{
	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}
	
	MenuManager->OpenMenu(MenuManager->GetAccessibilityMenu(), false);
}
