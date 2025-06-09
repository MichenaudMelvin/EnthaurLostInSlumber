// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Camera/ViewBobbing.h"
#include "Shakes/WaveOscillatorCameraShakePattern.h"

UViewBobbing::UViewBobbing(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bSingleInstance = true;

	ShakePattern = CreateDefaultSubobject<UWaveOscillatorCameraShakePattern>(TEXT("ShakePattern"));
	ShakePattern->Duration = 0.0f;

	ShakePattern->X.Amplitude = 0.0f;
	ShakePattern->Y.Amplitude = 0.0f;
	ShakePattern->Z.Amplitude = 5.0f;
	ShakePattern->Z.InitialOffsetType = EInitialWaveOscillatorOffsetType::Zero;

	SetRootShakePattern(ShakePattern);
}

FWaveOscillator UViewBobbing::GetOscillator() const
{
	if (!ShakePattern)
	{
		return GetEmptyOscillator();
	}

	return ShakePattern->Z;
}

float UViewBobbing::GetLocationAmplitudeMultiplier() const
{
	if (!ShakePattern)
	{
		return 0.0f;
	}

	return ShakePattern->LocationAmplitudeMultiplier;
}

float UViewBobbing::GetLocationFrequencyMultiplier() const
{
	if (!ShakePattern)
	{
		return 0.0f;
	}

	return ShakePattern->LocationFrequencyMultiplier;
}

FWaveOscillator UViewBobbing::GetEmptyOscillator()
{
	FWaveOscillator DefaultOscillator;
	DefaultOscillator.Amplitude = 0.0f;
	DefaultOscillator.Frequency = 0.0f;
	DefaultOscillator.InitialOffsetType = EInitialWaveOscillatorOffsetType::Zero;
	return DefaultOscillator;
}

void UViewBobbing::SetOscillator(const FWaveOscillator& Oscillator, float InLocationAmplitudeMultiplier, float InLocationFrequencyMultiplier) const
{
	if (!ShakePattern)
	{
		return;
	}

	ShakePattern->LocationAmplitudeMultiplier = InLocationAmplitudeMultiplier;
	ShakePattern->LocationFrequencyMultiplier = InLocationFrequencyMultiplier;
	ShakePattern->Z = Oscillator;
}
