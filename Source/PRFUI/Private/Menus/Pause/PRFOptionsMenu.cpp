// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Pause/PRFOptionsMenu.h"

#include "Components/Button.h"

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
	if (InvertMouseAxisButton)
	{
		InvertMouseAxisButton->OnHovered.AddDynamic(this, &UPRFOptionsMenu::OnMouseInvertButtonHovered);
	}
	if (ViewBobbingButton)
	{
		ViewBobbingButton->OnHovered.AddDynamic(this, &UPRFOptionsMenu::OnViewBobbingButtonHovered);
	}
	if (ViewControlsButton)
	{
		ViewControlsButton->OnHovered.AddDynamic(this, &UPRFOptionsMenu::OnViewControlsButton);
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
	if (InvertMouseAxisButton)
	{
		InvertMouseAxisButton->OnHovered.RemoveDynamic(this, &UPRFOptionsMenu::OnMouseInvertButtonHovered);
	}
	if (ViewBobbingButton)
	{
		ViewBobbingButton->OnHovered.RemoveDynamic(this, &UPRFOptionsMenu::OnViewBobbingButtonHovered);
	}
	if (ViewControlsButton)
	{
		ViewControlsButton->OnHovered.RemoveDynamic(this, &UPRFOptionsMenu::OnViewControlsButton);
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

void UPRFOptionsMenu::OnViewControlsButton()
{
	if (OptionTitle && OptionDescription)
	{
		OptionTitle->SetText(NSLOCTEXT("UI", "OptionTitleText", "View Controls"));
		OptionDescription->SetText(NSLOCTEXT("UI", "OptionDescriptionText", "View the available control schemes."));
	}
}
