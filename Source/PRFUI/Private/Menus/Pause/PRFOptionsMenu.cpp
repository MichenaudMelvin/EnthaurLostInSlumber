// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Pause/PRFOptionsMenu.h"
#include "PRFUIManager.h"
#include "Config/ENTUIConfig.h"
#include "Components/Button.h"
#include "Kismet/KismetTextLibrary.h"
#include "Saves/ENTSettingsSave.h"
#include "Subsystems/ENTSettingsSaveSubsystem.h"

void UPRFOptionsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (OverallVolumeSlider && OverallVolumeSlider->GetCustomSlider())
	{
		OverallVolumeSlider->GetCustomSlider()->OnValueChanged.AddDynamic(this, &UPRFOptionsMenu::OnOverallSliderChanged);
	}

	if (OverallVolumeButton && OverallVolumeButton->GetCustomButton())
	{
		OverallVolumeButton->GetCustomButton()->OnHovered.AddDynamic(this, &UPRFOptionsMenu::OnOverallButtonHovered);
	}
	if (MouseSensitivityButton && MouseSensitivityButton->GetCustomButton())
	{
		MouseSensitivityButton->GetCustomButton()->OnHovered.AddDynamic(this, &UPRFOptionsMenu::OnMouseSensButtonHovered);
	}
	if (MouseSensitivitySlider && MouseSensitivitySlider->GetCustomSlider())
	{
		MouseSensitivitySlider->GetCustomSlider()->OnValueChanged.AddDynamic(this, &UPRFOptionsMenu::OnMouseSensitivitySliderChanged);
	}
	if (InvertMouseAxisButton && InvertMouseAxisButton->GetCustomButton())
	{
		InvertMouseAxisButton->GetCustomButton()->OnHovered.AddDynamic(this, &UPRFOptionsMenu::OnMouseInvertButtonHovered);
	}
	if (InvertMouseAxisCheckBox)
	{
		InvertMouseAxisCheckBox->OnCheckBoxStateChanged.AddDynamic(this, &UPRFOptionsMenu::OnMouseYAxisCheckBoxClicked);
	}
	if (ViewBobbingButton && ViewBobbingButton->GetCustomButton())
	{
		ViewBobbingButton->GetCustomButton()->OnHovered.AddDynamic(this, &UPRFOptionsMenu::OnViewBobbingButtonHovered);
	}
	if (ViewBobbingCheckbox)
	{
		ViewBobbingCheckbox->OnCheckBoxStateChanged.AddDynamic(this, &UPRFOptionsMenu::OnViewBobbingCheckBoxClicked);
	}
	if (ViewControlsButton && ViewControlsButton->GetCustomButton())
	{
		ViewControlsButton->GetCustomButton()->OnHovered.AddDynamic(this, &UPRFOptionsMenu::OnViewControlsButtonHovered);
		ViewControlsButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFOptionsMenu::OnViewControlsButtonClicked);
	}

	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFOptionsMenu::ResetSettings);
	}
}

void UPRFOptionsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	OnOverallButtonHovered();

	UpdateWidgetValues(true);
}

void UPRFOptionsMenu::NativeDestruct()
{
	Super::NativeDestruct();

	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->SaveToSlot(0);
}

void UPRFOptionsMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (OverallVolumeSlider && OverallVolumeSlider->GetCustomSlider())
	{
		OverallVolumeSlider->GetCustomSlider()->OnValueChanged.RemoveDynamic(this, &UPRFOptionsMenu::OnOverallSliderChanged);
	}

	if (OverallVolumeButton && OverallVolumeButton->GetCustomButton())
	{
		OverallVolumeButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UPRFOptionsMenu::OnOverallButtonHovered);
	}
	if (MouseSensitivityButton && MouseSensitivityButton->GetCustomButton())
	{
		MouseSensitivityButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UPRFOptionsMenu::OnMouseSensButtonHovered);
	}
	if (MouseSensitivitySlider && MouseSensitivitySlider->GetCustomSlider())
	{
		MouseSensitivitySlider->GetCustomSlider()->OnValueChanged.RemoveDynamic(this, &UPRFOptionsMenu::OnMouseSensitivitySliderChanged);
	}
	if (InvertMouseAxisButton && InvertMouseAxisButton->GetCustomButton())
	{
		InvertMouseAxisButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UPRFOptionsMenu::OnMouseInvertButtonHovered);
	}
	if (InvertMouseAxisCheckBox)
	{
		InvertMouseAxisCheckBox->OnCheckBoxStateChanged.RemoveDynamic(this, &UPRFOptionsMenu::OnMouseYAxisCheckBoxClicked);
	}
	if (ViewBobbingButton && ViewBobbingButton->GetCustomButton())
	{
		ViewBobbingButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UPRFOptionsMenu::OnViewBobbingButtonHovered);
	}
	if (ViewBobbingCheckbox)
	{
		ViewBobbingCheckbox->OnCheckBoxStateChanged.RemoveDynamic(this, &UPRFOptionsMenu::OnViewBobbingCheckBoxClicked);
	}
	if (ViewControlsButton && ViewControlsButton->GetCustomButton())
	{
		ViewControlsButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UPRFOptionsMenu::OnViewControlsButtonHovered);
		ViewControlsButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFOptionsMenu::OnViewControlsButtonClicked);
	}

	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFOptionsMenu::ResetSettings);
	}
}

void UPRFOptionsMenu::UpdateWidgetValues(bool bSkipAnim)
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

void UPRFOptionsMenu::OnOverallButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "Overall Volume"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Adjust the volume of all audio."));
	}
}

void UPRFOptionsMenu::OnMouseSensButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "Mouse Sensitivity"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Adjust the aiming speed of the camera."));
	}
}

void UPRFOptionsMenu::OnMouseInvertButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "Invert Mouse Y Axis"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Invert the vertical rotation direction of the camera."));
	}
}

void UPRFOptionsMenu::OnViewBobbingButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "View Bobbing"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Enable or disable view bobbing."));
	}
}

void UPRFOptionsMenu::OnViewControlsButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "View Controls"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "View the available control schemes."));
	}
}

void UPRFOptionsMenu::OnViewControlsButtonClicked()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}
	
	const UENTUIConfig* UIManagerSettings = GetDefault<UENTUIConfig>();
	if (!IsValid(UIManagerSettings))
	{
		return;
	}

	UUserWidget* ControlsMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->ControlsMenuClass);
	UIManager->OpenMenu(ControlsMenu, false);
}

void UPRFOptionsMenu::OnOverallSliderChanged(float InValue)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->SetMasterVolume(InValue);
	OverallVolumeValue->SetText(UKismetTextLibrary::Conv_DoubleToText(InValue, HalfToEven, false, true, 1, 3, 0, 0));
}

void UPRFOptionsMenu::OnViewBobbingCheckBoxClicked(bool bIsChecked, bool bSkip)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->GetSettings()->bViewBobbing = bIsChecked;
}

void UPRFOptionsMenu::OnMouseYAxisCheckBoxClicked(bool bIsChecked, bool bSkip)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->GetSettings()->bInvertYAxis = bIsChecked;
}

void UPRFOptionsMenu::OnMouseSensitivitySliderChanged(float InValue)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->GetSettings()->MouseSensitivity = InValue;
	MouseSensitivityValue->SetText(UKismetTextLibrary::Conv_DoubleToText(InValue, HalfToEven, false, true, 1, 2, 1, 1));
}

void UPRFOptionsMenu::ResetSettings()
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->ResetSaveToDefault(0);
	UpdateWidgetValues(false);
}
