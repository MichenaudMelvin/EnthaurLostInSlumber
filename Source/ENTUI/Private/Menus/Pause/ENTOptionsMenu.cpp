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

	if (OverallVolumeSlider && OverallVolumeSlider->GetCustomSlider())
	{
		OverallVolumeSlider->GetCustomSlider()->OnValueChanged.AddDynamic(this, &UENTOptionsMenu::OnOverallSliderChanged);
	}

	if (OverallVolumeButton && OverallVolumeButton->GetCustomButton())
	{
		OverallVolumeButton->GetCustomButton()->OnHovered.AddDynamic(this, &UENTOptionsMenu::OnOverallButtonHovered);
	}
	if (MouseSensitivityButton && MouseSensitivityButton->GetCustomButton())
	{
		MouseSensitivityButton->GetCustomButton()->OnHovered.AddDynamic(this, &UENTOptionsMenu::OnMouseSensButtonHovered);
	}
	if (MouseSensitivitySlider && MouseSensitivitySlider->GetCustomSlider())
	{
		MouseSensitivitySlider->GetCustomSlider()->OnValueChanged.AddDynamic(this, &UENTOptionsMenu::OnMouseSensitivitySliderChanged);
	}
	if (InvertMouseAxisButton && InvertMouseAxisButton->GetCustomButton())
	{
		InvertMouseAxisButton->GetCustomButton()->OnHovered.AddDynamic(this, &UENTOptionsMenu::OnMouseInvertButtonHovered);
	}
	if (InvertMouseAxisCheckBox)
	{
		InvertMouseAxisCheckBox->OnCheckBoxStateChanged.AddDynamic(this, &UENTOptionsMenu::OnMouseYAxisCheckBoxClicked);
	}
	if (ViewBobbingButton && ViewBobbingButton->GetCustomButton())
	{
		ViewBobbingButton->GetCustomButton()->OnHovered.AddDynamic(this, &UENTOptionsMenu::OnViewBobbingButtonHovered);
	}
	if (ViewBobbingCheckbox)
	{
		ViewBobbingCheckbox->OnCheckBoxStateChanged.AddDynamic(this, &UENTOptionsMenu::OnViewBobbingCheckBoxClicked);
	}
	if (ViewControlsButton && ViewControlsButton->GetCustomButton())
	{
		ViewControlsButton->GetCustomButton()->OnHovered.AddDynamic(this, &UENTOptionsMenu::OnViewControlsButtonHovered);
		ViewControlsButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTOptionsMenu::OnViewControlsButtonClicked);
	}

	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTOptionsMenu::ResetSettings);
	}
}

void UENTOptionsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	OnOverallButtonHovered();

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

	if (OverallVolumeSlider && OverallVolumeSlider->GetCustomSlider())
	{
		OverallVolumeSlider->GetCustomSlider()->OnValueChanged.RemoveDynamic(this, &UENTOptionsMenu::OnOverallSliderChanged);
	}

	if (OverallVolumeButton && OverallVolumeButton->GetCustomButton())
	{
		OverallVolumeButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UENTOptionsMenu::OnOverallButtonHovered);
	}
	if (MouseSensitivityButton && MouseSensitivityButton->GetCustomButton())
	{
		MouseSensitivityButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UENTOptionsMenu::OnMouseSensButtonHovered);
	}
	if (MouseSensitivitySlider && MouseSensitivitySlider->GetCustomSlider())
	{
		MouseSensitivitySlider->GetCustomSlider()->OnValueChanged.RemoveDynamic(this, &UENTOptionsMenu::OnMouseSensitivitySliderChanged);
	}
	if (InvertMouseAxisButton && InvertMouseAxisButton->GetCustomButton())
	{
		InvertMouseAxisButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UENTOptionsMenu::OnMouseInvertButtonHovered);
	}
	if (InvertMouseAxisCheckBox)
	{
		InvertMouseAxisCheckBox->OnCheckBoxStateChanged.RemoveDynamic(this, &UENTOptionsMenu::OnMouseYAxisCheckBoxClicked);
	}
	if (ViewBobbingButton && ViewBobbingButton->GetCustomButton())
	{
		ViewBobbingButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UENTOptionsMenu::OnViewBobbingButtonHovered);
	}
	if (ViewBobbingCheckbox)
	{
		ViewBobbingCheckbox->OnCheckBoxStateChanged.RemoveDynamic(this, &UENTOptionsMenu::OnViewBobbingCheckBoxClicked);
	}
	if (ViewControlsButton && ViewControlsButton->GetCustomButton())
	{
		ViewControlsButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UENTOptionsMenu::OnViewControlsButtonHovered);
		ViewControlsButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTOptionsMenu::OnViewControlsButtonClicked);
	}

	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTOptionsMenu::ResetSettings);
	}
}

void UENTOptionsMenu::UpdateWidgetValues(bool bSkipAnim)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!IsValid(SettingsSubsystem))
	{
		return;
	}

	OverallVolumeSlider->GetCustomSlider()->SetValue(SettingsSubsystem->GetSettings()->MasterVolume);
	MouseSensitivitySlider->GetCustomSlider()->SetValue(SettingsSubsystem->GetSettings()->MouseSensitivity);
	InvertMouseAxisCheckBox->SetIsOn(SettingsSubsystem->GetSettings()->bInvertYAxis, bSkipAnim);
	ViewBobbingCheckbox->SetIsOn(SettingsSubsystem->GetSettings()->bViewBobbing, bSkipAnim);
}

void UENTOptionsMenu::OnOverallButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "Overall Volume"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Adjust the volume of all audio."));
	}
}

void UENTOptionsMenu::OnMouseSensButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "Mouse Sensitivity"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Adjust the aiming speed of the camera."));
	}
}

void UENTOptionsMenu::OnMouseInvertButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "Invert Mouse Y Axis"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Invert the vertical rotation direction of the camera."));
	}
}

void UENTOptionsMenu::OnViewBobbingButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "View Bobbing"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Enable or disable view bobbing."));
	}
}

void UENTOptionsMenu::OnViewControlsButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "View Controls"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "View the available control schemes."));
	}
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

void UENTOptionsMenu::OnOverallSliderChanged(float InValue)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->SetMasterVolume(InValue);
	OverallVolumeValue->SetText(UKismetTextLibrary::Conv_DoubleToText(InValue, HalfToEven, false, true, 1, 3, 0, 0));
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

void UENTOptionsMenu::ResetSettings()
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->ResetSaveToDefault(0);
	UpdateWidgetValues(false);
}
