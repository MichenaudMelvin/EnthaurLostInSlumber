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
	if (MouseSensitivitySlider && MouseSensitivitySlider->GetCustomSlider())
	{
		MouseSensitivitySlider->GetCustomSlider()->OnValueChanged.AddDynamic(this, &UENTOptionsMenu::OnMouseSensitivitySliderChanged);
	}
	if (InvertMouseAxisButton && InvertMouseAxisButton->GetCustomButton())
	if (InvertMouseAxisCheckBox)
	{
		InvertMouseAxisCheckBox->OnCheckBoxStateChanged.AddDynamic(this, &UENTOptionsMenu::OnMouseYAxisCheckBoxClicked);
	}
	if (ViewBobbingCheckbox)
	{
		ViewBobbingCheckbox->OnCheckBoxStateChanged.AddDynamic(this, &UENTOptionsMenu::OnViewBobbingCheckBoxClicked);
	}
	if (ControlsButton && ControlsButton->GetCustomButton())
	{
		ControlsButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTOptionsMenu::OnViewControlsButtonClicked);
	}
	if (GammaButton && GammaButton->GetCustomButton())
	{
		GammaButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTOptionsMenu::OnGammaButtonClicked);
	}
}

void UENTOptionsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateWidgetValues(true);
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
	if (MouseSensitivitySlider && MouseSensitivitySlider->GetCustomSlider())
	{
		MouseSensitivitySlider->GetCustomSlider()->OnValueChanged.RemoveDynamic(this, &UENTOptionsMenu::OnMouseSensitivitySliderChanged);
	}
	if (InvertMouseAxisCheckBox)
	{
		InvertMouseAxisCheckBox->OnCheckBoxStateChanged.RemoveDynamic(this, &UENTOptionsMenu::OnMouseYAxisCheckBoxClicked);
	}
	if (ViewBobbingCheckbox)
	{
		ViewBobbingCheckbox->OnCheckBoxStateChanged.RemoveDynamic(this, &UENTOptionsMenu::OnViewBobbingCheckBoxClicked);
	}
	if (ControlsButton && ControlsButton->GetCustomButton())
	{
		ControlsButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTOptionsMenu::OnViewControlsButtonClicked);
	}
	if (GammaButton && GammaButton->GetCustomButton())
	{
		GammaButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTOptionsMenu::OnGammaButtonClicked);
	}
}

void UENTOptionsMenu::UpdateWidgetValues(bool bSkipAnim)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!IsValid(SettingsSubsystem))
	{
		return;
	}
	
	MouseSensitivitySlider->GetCustomSlider()->SetValue(SettingsSubsystem->GetSettings()->MouseSensitivity);
	InvertMouseAxisCheckBox->SetIsOn(SettingsSubsystem->GetSettings()->bInvertYAxis, bSkipAnim);
	ViewBobbingCheckbox->SetIsOn(SettingsSubsystem->GetSettings()->bViewBobbing, bSkipAnim);
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

void UENTOptionsMenu::OnViewBobbingCheckBoxClicked(bool bIsChecked, bool bSkip)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->GetSettings()->bViewBobbing = bIsChecked;
}

void UENTOptionsMenu::OnMouseYAxisCheckBoxClicked(bool bIsChecked, bool bSkip)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->GetSettings()->bInvertYAxis = bIsChecked;
}

void UENTOptionsMenu::OnMouseSensitivitySliderChanged(float InValue)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->GetSettings()->MouseSensitivity = InValue;
	MouseSensitivityValue->SetText(UKismetTextLibrary::Conv_DoubleToText(InValue, HalfToEven, false, true, 1, 2, 1, 1));
}
