// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Options/ENTGammaMenu.h"

#include "Menus/Elements/ENTCustomButton.h"
#include "Menus/Options/ENTResetConfirmationMenu.h"
#include "Saves/ENTSettingsSave.h"
#include "Subsystems/ENTMenuManager.h"
#include "Subsystems/ENTSettingsSaveSubsystem.h"
#include "Util/ColorConstants.h"

void UENTGammaMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (GammaSlider && GammaSlider->GetCustomSlider())
	{
		GammaSlider->GetCustomSlider()->OnValueChanged.AddDynamic(this, &UENTGammaMenu::OnGammaSliderValueChanged);
	}
	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTGammaMenu::OpenResetSettingsMenu);
	}
}

void UENTGammaMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	UpdateWidgetValues();
}

void UENTGammaMenu::BeginDestroy()
{
	Super::BeginDestroy();
	
	if (GammaSlider && GammaSlider->GetCustomSlider())
	{
		GammaSlider->GetCustomSlider()->OnValueChanged.RemoveDynamic(this, &UENTGammaMenu::OnGammaSliderValueChanged);
	}
	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTGammaMenu::OpenResetSettingsMenu);
	}
}

void UENTGammaMenu::OnGammaSliderValueChanged(float InValue)
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return;
	}
	
	SettingsSubsystem->SetGamma(InValue);
	
	float floatA = GammaSlider->GetCustomSlider()->GetValue() - GammaSlider->GetCustomSlider()->GetMinValue();
	float floatB = GammaSlider->GetCustomSlider()->GetMaxValue() - GammaSlider->GetCustomSlider()->GetMinValue();
	
	float normalizedVectorValue = (floatA / floatB);
	int brightnessOffset = 80;
	int normalizedVectorValueInt = FMath::Lerp(0, 255, normalizedVectorValue) - brightnessOffset;
	
	if (normalizedVectorValueInt < 0)
	{
		normalizedVectorValueInt = 0;
	}
	
	FSlateColor UpdatedColor = FSlateColor(FColor(normalizedVectorValueInt, normalizedVectorValueInt, normalizedVectorValueInt, normalizedVectorValueInt));
	GammaImage->SetBrushTintColor(UpdatedColor);
}

void UENTGammaMenu::UpdateWidgetValues()
{
	UENTSettingsSaveSubsystem* SettingsSubsystem = GetGameInstance()->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!IsValid(SettingsSubsystem))
	{
		return;
	}
	
	GammaSlider->GetCustomSlider()->SetValue(SettingsSubsystem->GetSettings()->Gamma);
}

void UENTGammaMenu::OpenResetSettingsMenu()
{
	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}

	UENTResetConfirmationMenu* ResetConfirmationMenu = Cast<UENTResetConfirmationMenu>(MenuManager->GetResetConfirmationMenu());
	if (!IsValid(ResetConfirmationMenu))
	{
		return;
	}

	ResetConfirmationMenu->SetMenuType(EENTResetMenuType::Brightness);
	MenuManager->OpenMenu(MenuManager->GetResetConfirmationMenu(), false);
}
