// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Options/ENTAccessibilityMenu.h"

#include "Kismet/KismetTextLibrary.h"
#include "Menus/Elements/ENTCustomButton.h"
#include "Saves/ENTSettingsSave.h"
#include "Subsystems/ENTMenuManager.h"
#include "Subsystems/ENTSettingsSaveSubsystem.h"

void UENTAccessibilityMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (MouseSensitivitySlider && MouseSensitivitySlider->GetCustomSlider())
	{
		MouseSensitivitySlider->GetCustomSlider()->OnValueChanged.AddDynamic(this, &UENTAccessibilityMenu::OnMouseSensitivitySliderChanged);
	}

	if (MouseSensitivityButton && MouseSensitivityButton->GetCustomButton())
	{
		MouseSensitivityButton->GetCustomButton()->OnHovered.AddDynamic(this, &UENTAccessibilityMenu::OnMouseSensitivityButtonHovered);
	}
	if (InvertMouseAxisButton && InvertMouseAxisButton->GetCustomButton())
	{
		InvertMouseAxisButton->GetCustomButton()->OnHovered.AddDynamic(this, &UENTAccessibilityMenu::OnInvertMouseAxisButtonHovered);
	}
	if (ViewBobbingButton && ViewBobbingButton->GetCustomButton())
	{
		ViewBobbingButton->GetCustomButton()->OnHovered.AddDynamic(this, &UENTAccessibilityMenu::OnViewBobbingButtonHovered);
	}
	
	if (InvertMouseAxisCheckBox)
	{
		InvertMouseAxisCheckBox->OnCheckBoxStateChanged.AddDynamic(this, &UENTAccessibilityMenu::OnMouseYAxisCheckBoxClicked);
	}
	if (ViewBobbingCheckbox)
	{
		ViewBobbingCheckbox->OnCheckBoxStateChanged.AddDynamic(this, &UENTAccessibilityMenu::OnViewBobbingCheckBoxClicked);
	}
	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTAccessibilityMenu::OpenResetSettingsMenu);
	}
}

void UENTAccessibilityMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!IsValid(SettingsSubsystem))
	{
		return;
	}

	OnMouseSensitivityButtonHovered();

	MouseSensitivitySlider->GetCustomSlider()->SetValue(SettingsSubsystem->GetSettings()->MouseSensitivity);
	InvertMouseAxisCheckBox->SetIsOn(SettingsSubsystem->GetSettings()->bInvertYAxis, true);
	ViewBobbingCheckbox->SetIsOn(SettingsSubsystem->GetSettings()->bViewBobbing, true);
}

void UENTAccessibilityMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (MouseSensitivitySlider && MouseSensitivitySlider->GetCustomSlider())
	{
		MouseSensitivitySlider->GetCustomSlider()->OnValueChanged.RemoveDynamic(this, &UENTAccessibilityMenu::OnMouseSensitivitySliderChanged);
	}

	if (MouseSensitivityButton && MouseSensitivityButton->GetCustomButton())
	{
		MouseSensitivityButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UENTAccessibilityMenu::OnMouseSensitivityButtonHovered);
	}
	if (InvertMouseAxisButton && InvertMouseAxisButton->GetCustomButton())
	{
		InvertMouseAxisButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UENTAccessibilityMenu::OnInvertMouseAxisButtonHovered);
	}
	if (ViewBobbingButton && ViewBobbingButton->GetCustomButton())
	{
		ViewBobbingButton->GetCustomButton()->OnHovered.RemoveDynamic(this, &UENTAccessibilityMenu::OnViewBobbingButtonHovered);
	}
	
	if (InvertMouseAxisCheckBox)
	{
		InvertMouseAxisCheckBox->OnCheckBoxStateChanged.RemoveDynamic(this, &UENTAccessibilityMenu::OnMouseYAxisCheckBoxClicked);
	}
	if (ViewBobbingCheckbox)
	{
		ViewBobbingCheckbox->OnCheckBoxStateChanged.RemoveDynamic(this, &UENTAccessibilityMenu::OnViewBobbingCheckBoxClicked);
	}
	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTAccessibilityMenu::OpenResetSettingsMenu);
	}
}

void UENTAccessibilityMenu::OnMouseSensitivitySliderChanged(float InValue)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->GetSettings()->MouseSensitivity = InValue;
	MouseSensitivityValue->SetText(UKismetTextLibrary::Conv_DoubleToText(InValue, HalfToEven, false, true, 1, 2, 1, 1));
}

void UENTAccessibilityMenu::OnMouseSensitivityButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "Mouse Sensitivity"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Adjust X & Y axis mouse sensitivity."));
	}
}

void UENTAccessibilityMenu::OnInvertMouseAxisButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "Invert Mouse Y Axis"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Mirror the mouse Y axis movement."));
	}
}

void UENTAccessibilityMenu::OnViewBobbingButtonHovered()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "View Bobbing"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "Toggle the character's camera movement."));
	}
}

void UENTAccessibilityMenu::OnMouseYAxisCheckBoxClicked(bool bIsChecked, bool bSkip)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->GetSettings()->bInvertYAxis = bIsChecked;
}

void UENTAccessibilityMenu::OnViewBobbingCheckBoxClicked(bool bIsChecked, bool bSkip)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->GetSettings()->bViewBobbing = bIsChecked;
}

void UENTAccessibilityMenu::OpenResetSettingsMenu()
{
	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}

	MenuManager->OpenMenu(MenuManager->GetResetConfirmationMenu(), false);
}
