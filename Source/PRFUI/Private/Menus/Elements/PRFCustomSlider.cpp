// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Elements/PRFCustomSlider.h"

#include "Kismet/KismetMathLibrary.h"

void UPRFCustomSlider::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (CustomSlider)
	{
		CustomSlider->OnValueChanged.AddDynamic(this, &UPRFCustomSlider::OnCustomSliderValueChanged);
	}
}

void UPRFCustomSlider::BeginDestroy()
{
	Super::BeginDestroy();

	if (CustomSlider)
	{
		CustomSlider->OnValueChanged.RemoveDynamic(this, &UPRFCustomSlider::OnCustomSliderValueChanged);
	}
}

void UPRFCustomSlider::OnCustomSliderValueChanged(float InValue)
{
	if (ProgressBarBackground)
	{
		ProgressBarBackground->SetPercent(UKismetMathLibrary::NormalizeToRange(InValue, CustomSlider->GetMinValue(), CustomSlider->GetMaxValue()));
	}
}
