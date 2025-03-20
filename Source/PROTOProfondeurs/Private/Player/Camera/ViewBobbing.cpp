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

FWaveOscillator UViewBobbing::GetEmptyOscillator()
{
	FWaveOscillator DefaultOscillator;
	DefaultOscillator.Amplitude = 0.0f;
	DefaultOscillator.Frequency = 0.0f;
	DefaultOscillator.InitialOffsetType = EInitialWaveOscillatorOffsetType::Zero;
	return DefaultOscillator;
}

void UViewBobbing::SetOscillator(const FWaveOscillator& Oscillator) const
{
	if (!ShakePattern)
	{
		return;
	}

	ShakePattern->Z = Oscillator;
}
