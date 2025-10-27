// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Camera/ENTViewBobbing.h"
#include "Shakes/WaveOscillatorCameraShakePattern.h"

UENTViewBobbing::UENTViewBobbing(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bSingleInstance = true;

	ShakePattern = CreateDefaultSubobject<UWaveOscillatorCameraShakePattern>(TEXT("ShakePattern"));
	ShakePattern->Duration = 0.0f;

	ShakePattern->X.Amplitude = 0.0f;
	ShakePattern->Y.Amplitude = 0.0f;
	ShakePattern->Z.Amplitude = 5.0f;
	ShakePattern->Z.InitialOffsetType = EInitialWaveOscillatorOffsetType::Zero;

	ShakePattern->Yaw.Amplitude = 0.f;
	ShakePattern->Pitch.Amplitude = 0.f;
	ShakePattern->Roll.InitialOffsetType = EInitialWaveOscillatorOffsetType::Zero;

	SetRootShakePattern(ShakePattern);
}

FWaveOscillator UENTViewBobbing::GetOscillator() const
{
	if (!ShakePattern)
	{
		return GetEmptyOscillator();
	}

	return ShakePattern->Z;
}

FWaveOscillator UENTViewBobbing::GetRollOscillator() const
{
	if (!ShakePattern)
		return GetEmptyOscillator();

	return ShakePattern->Roll;
}

float UENTViewBobbing::GetLocationAmplitudeMultiplier() const
{
	if (!ShakePattern)
	{
		return 0.0f;
	}

	return ShakePattern->LocationAmplitudeMultiplier;
}

float UENTViewBobbing::GetRotationAmplitudeMultiplier() const
{
	if (!ShakePattern)
		return 0.0f;

	return ShakePattern->RotationAmplitudeMultiplier;
}

float UENTViewBobbing::GetLocationFrequencyMultiplier() const
{
	if (!ShakePattern)
	{
		return 0.0f;
	}

	return ShakePattern->LocationFrequencyMultiplier;
}

FWaveOscillator UENTViewBobbing::GetEmptyOscillator()
{
	FWaveOscillator DefaultOscillator;
	DefaultOscillator.Amplitude = 0.0f;
	DefaultOscillator.Frequency = 0.0f;
	DefaultOscillator.InitialOffsetType = EInitialWaveOscillatorOffsetType::Zero;
	return DefaultOscillator;
}

void UENTViewBobbing::SetLocationOscillator(const FWaveOscillator& Oscillator, float InLocationAmplitudeMultiplier, float InLocationFrequencyMultiplier) const
{
	if (!ShakePattern)
		return;

	ShakePattern->LocationAmplitudeMultiplier = InLocationAmplitudeMultiplier;
	ShakePattern->LocationFrequencyMultiplier = InLocationFrequencyMultiplier;
	ShakePattern->Z = Oscillator;
}

void UENTViewBobbing::SetRollOscillator(const FWaveOscillator& Oscillator, float InRotationAmplitudeMultiplier, float InRotationFrequencyMultiplier) const
{
	if (!ShakePattern)
		return;
	
	ShakePattern->RotationAmplitudeMultiplier = InRotationAmplitudeMultiplier;
	ShakePattern->RotationFrequencyMultiplier = InRotationFrequencyMultiplier;
	ShakePattern->Roll = Oscillator;
}
