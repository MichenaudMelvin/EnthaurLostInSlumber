// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Pause/PRFOptionsMenu.h"

#include "PRFUIManager.h"
#include "UIManagerSettings.h"
#include "Components/Button.h"
#include "Saves/SettingsSubsystem.h"

void UPRFOptionsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (OverallVolumeButton)
	{
		OverallVolumeButton->OnHovered.AddDynamic(this, &UPRFOptionsMenu::OnOverallButtonHovered);
	}
	if (MouseSensitivityButton)
	{
		MouseSensitivityButton->OnHovered.AddDynamic(this, &UPRFOptionsMenu::OnMouseSensButtonHovered);
	}
	if (MouseSensitivitySlider)
	{
		MouseSensitivitySlider->OnValueChanged.AddDynamic(this, &UPRFOptionsMenu::UPRFOptionsMenu::OnMouseSensitivitySliderChanged);
	}
	if (InvertMouseAxisButton)
	{
		InvertMouseAxisButton->OnHovered.AddDynamic(this, &UPRFOptionsMenu::OnMouseInvertButtonHovered);
	}
	if (InvertMouseAxisCheckBox)
	{
		InvertMouseAxisCheckBox->OnCheckStateChanged.AddDynamic(this, &UPRFOptionsMenu::OnMouseYAxisCheckBoxClicked);
	}
	if (ViewBobbingButton)
	{
		ViewBobbingButton->OnHovered.AddDynamic(this, &UPRFOptionsMenu::OnViewBobbingButtonHovered);
	}
	if (ViewBobbingCheckbox)
	{
		ViewBobbingCheckbox->OnCheckStateChanged.AddDynamic(this, &UPRFOptionsMenu::OnViewBobbingCheckBoxClicked);
	}
	if (ViewControlsButton)
	{
		ViewControlsButton->OnHovered.AddDynamic(this, &UPRFOptionsMenu::OnViewControlsButtonHovered);
		ViewControlsButton->OnClicked.AddDynamic(this, &UPRFOptionsMenu::OnViewControlsButtonClicked);
	}
}

void UPRFOptionsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	OnOverallButtonHovered();
}

void UPRFOptionsMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (OverallVolumeButton)
	{
		OverallVolumeButton->OnHovered.RemoveDynamic(this, &UPRFOptionsMenu::OnOverallButtonHovered);
	}
	if (MouseSensitivityButton)
	{
		MouseSensitivityButton->OnHovered.RemoveDynamic(this, &UPRFOptionsMenu::OnMouseSensButtonHovered);
	}
	if (MouseSensitivitySlider)
	{
		MouseSensitivitySlider->OnValueChanged.RemoveDynamic(this, &UPRFOptionsMenu::UPRFOptionsMenu::OnMouseSensitivitySliderChanged);
	}
	if (InvertMouseAxisButton)
	{
		InvertMouseAxisButton->OnHovered.RemoveDynamic(this, &UPRFOptionsMenu::OnMouseInvertButtonHovered);
	}
	if (InvertMouseAxisCheckBox)
	{
		InvertMouseAxisCheckBox->OnCheckStateChanged.RemoveDynamic(this, &UPRFOptionsMenu::OnMouseYAxisCheckBoxClicked);
	}
	if (ViewBobbingButton)
	{
		ViewBobbingButton->OnHovered.RemoveDynamic(this, &UPRFOptionsMenu::OnViewBobbingButtonHovered);
	}
	if (ViewBobbingCheckbox)
	{
		ViewBobbingCheckbox->OnCheckStateChanged.RemoveDynamic(this, &UPRFOptionsMenu::OnViewBobbingCheckBoxClicked);
	}
	if (ViewControlsButton)
	{
		ViewControlsButton->OnHovered.RemoveDynamic(this, &UPRFOptionsMenu::OnViewControlsButtonHovered);
		ViewControlsButton->OnClicked.RemoveDynamic(this, &UPRFOptionsMenu::OnViewControlsButtonClicked);
	}
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
	
	const UUIManagerSettings* UIManagerSettings = GetDefault<UUIManagerSettings>();
	if (!IsValid(UIManagerSettings))
	{
		return;
	}

	UUserWidget* ControlsMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->ControlsMenuClass);
	UIManager->OpenMenu(ControlsMenu, false);
}

void UPRFOptionsMenu::OnOverallSliderChanged()
{

}

void UPRFOptionsMenu::OnMouseSensSliderChanged()
{
	
}

void UPRFOptionsMenu::OnViewBobbingCheckBoxClicked(bool bIsChecked)
{
	USettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<USettingsSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->GetSettings()->bViewBobbing = bIsChecked;
}

void UPRFOptionsMenu::OnMouseYAxisCheckBoxClicked(bool bIsChecked)
{
	USettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<USettingsSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->GetSettings()->bInvertYAxis = bIsChecked;
}

void UPRFOptionsMenu::OnMouseSensitivitySliderChanged(float InValue)
{
	USettingsSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<USettingsSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}

	SettingsSubsystem->GetSettings()->MouseSensitivity = InValue;
}
