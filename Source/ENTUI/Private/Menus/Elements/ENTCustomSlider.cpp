// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Elements/ENTCustomSlider.h"
#include "Kismet/KismetMathLibrary.h"

void UENTCustomSlider::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (CustomSlider)
	{
		CustomSlider->OnValueChanged.AddDynamic(this, &UENTCustomSlider::OnCustomSliderValueChanged);
	}
}

void UENTCustomSlider::BeginDestroy()
{
	Super::BeginDestroy();

	if (CustomSlider)
	{
		CustomSlider->OnValueChanged.RemoveDynamic(this, &UENTCustomSlider::OnCustomSliderValueChanged);
	}
}

void UENTCustomSlider::OnCustomSliderValueChanged(float InValue)
{
	if (ProgressBarBackground)
	{
		ProgressBarBackground->SetPercent(UKismetMathLibrary::NormalizeToRange(InValue, CustomSlider->GetMinValue(), CustomSlider->GetMaxValue()));
	}
}
