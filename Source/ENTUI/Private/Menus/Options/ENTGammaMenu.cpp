// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Options/ENTGammaMenu.h"

#include "Saves/ENTSettingsSave.h"
#include "Subsystems/ENTSettingsSaveSubsystem.h"
#include "Util/ColorConstants.h"

void UENTGammaMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (GammaSlider && GammaSlider->GetCustomSlider())
	{
		GammaSlider->GetCustomSlider()->OnValueChanged.AddDynamic(this, &UENTGammaMenu::OnGammaSliderValueChanged);
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
	int brightnessOffset = 120;
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
