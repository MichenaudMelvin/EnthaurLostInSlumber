// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Options/ENTSoundMenu.h"

#include "Kismet/KismetTextLibrary.h"
#include "Menus/Elements/ENTCustomButton.h"
#include "Menus/Elements/ENTCustomSlider.h"
#include "Saves/ENTSettingsSave.h"
#include "Subsystems/ENTMenuManager.h"
#include "Subsystems/ENTSettingsSaveSubsystem.h"

void UENTSoundMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (OverallVolumeSlider && OverallVolumeSlider->GetCustomSlider())
	{
		OverallVolumeSlider->GetCustomSlider()->OnValueChanged.AddDynamic(this, &UENTSoundMenu::OnOverallSliderChanged);
	}
	if (MusicVolumeSlider && MusicVolumeSlider->GetCustomSlider())
	{
		MusicVolumeSlider->GetCustomSlider()->OnValueChanged.AddDynamic(this, &UENTSoundMenu::OnMusicSliderChanged);
	}
	if (SfxVolumeSlider && SfxVolumeSlider->GetCustomSlider())
	{
		SfxVolumeSlider->GetCustomSlider()->OnValueChanged.AddDynamic(this, &UENTSoundMenu::OnSfxSliderChanged);
	}
	
	if (OverallVolumeButton && OverallVolumeButton->GetCustomButton())
	{
		OverallVolumeButton->GetCustomButton()->OnHovered.AddDynamic(this, &UENTSoundMenu::OnOverallButtonHovered);
	}
	if (MusicVolumeButton && MusicVolumeButton->GetCustomButton())
	{
		MusicVolumeButton->GetCustomButton()->OnHovered.AddDynamic(this, &UENTSoundMenu::OnMusicButtonHovered);
	}
	if (SfxVolumeButton && SfxVolumeButton->GetCustomButton())
	{
		SfxVolumeButton->GetCustomButton()->OnHovered.AddDynamic(this, &UENTSoundMenu::OnSfxButtonHovered);
	}
	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTSoundMenu::OpenResetSettingsMenu);
	}
}

void UENTSoundMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!IsValid(SettingsSubsystem))
	{
		return;
	}

	if (!OverallVolumeSlider->GetCustomSlider() || !MusicVolumeSlider->GetCustomSlider() || !SfxVolumeSlider->GetCustomSlider())
	{
		return;
	}

	OverallVolumeSlider->GetCustomSlider()->SetValue(SettingsSubsystem->GetSettings()->MasterVolume);
	MusicVolumeSlider->GetCustomSlider()->SetValue(SettingsSubsystem->GetSettings()->MusicVolume);
	SfxVolumeSlider->GetCustomSlider()->SetValue(SettingsSubsystem->GetSettings()->SFXVolume);

	OnOverallButtonHovered();
}

void UENTSoundMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (OverallVolumeSlider && OverallVolumeSlider->GetCustomSlider())
	{
		OverallVolumeSlider->GetCustomSlider()->OnValueChanged.RemoveDynamic(this, &UENTSoundMenu::OnOverallSliderChanged);
	}
	if (MusicVolumeSlider && MusicVolumeSlider->GetCustomSlider())
	{
		MusicVolumeSlider->GetCustomSlider()->OnValueChanged.RemoveDynamic(this, &UENTSoundMenu::OnMusicSliderChanged);
	}
	if (SfxVolumeSlider && SfxVolumeSlider->GetCustomSlider())
	{
		SfxVolumeSlider->GetCustomSlider()->OnValueChanged.RemoveDynamic(this, &UENTSoundMenu::OnSfxSliderChanged);
	}

	if (OverallVolumeButton && OverallVolumeButton->GetCustomButton())
	{
		OverallVolumeButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UENTSoundMenu::OnOverallButtonHovered);
	}
	if (MusicVolumeButton && MusicVolumeButton->GetCustomButton())
	{
		MusicVolumeButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UENTSoundMenu::OnMusicButtonHovered);
	}
	if (SfxVolumeButton && SfxVolumeButton->GetCustomButton())
	{
		SfxVolumeButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UENTSoundMenu::OnSfxButtonHovered);
	}
	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTSoundMenu::OpenResetSettingsMenu);
	}
}

void UENTSoundMenu::OnOverallSliderChanged(float InValue)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->SetMasterVolume(InValue);
	OverallVolumeValue->SetText(UKismetTextLibrary::Conv_DoubleToText(InValue, HalfToEven, false, true, 1, 3, 0, 0));
}

void UENTSoundMenu::OnMusicSliderChanged(float InValue)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->SetMusicVolume(InValue);
	MusicVolumeValue->SetText(UKismetTextLibrary::Conv_DoubleToText(InValue, HalfToEven, false, true, 1, 3, 0, 0));
}

void UENTSoundMenu::OnSfxSliderChanged(float InValue)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->SetSFXVolume(InValue);
	SfxVolumeValue->SetText(UKismetTextLibrary::Conv_DoubleToText(InValue, HalfToEven, false, true, 1, 3, 0, 0));
}

void UENTSoundMenu::OnOverallButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "Overall Volume"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Adjust the volume of all audio."));
	}
}

void UENTSoundMenu::OnMusicButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "Music Volume"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Adjust the volume of the music."));
	}
}

void UENTSoundMenu::OnSfxButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "SFX Volume"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Adjust the volume of the sound effects."));
	}
}

void UENTSoundMenu::OpenResetSettingsMenu()
{
	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}

	MenuManager->OpenMenu(MenuManager->GetResetConfirmationMenu(), false);
}
