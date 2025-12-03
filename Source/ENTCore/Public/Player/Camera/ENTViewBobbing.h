// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "ENTViewBobbing.generated.h"

class UWaveOscillatorCameraShakePattern;
struct FWaveOscillator;

UCLASS()
class ENTCORE_API UENTViewBobbing : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UENTViewBobbing(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY()
	TObjectPtr<UWaveOscillatorCameraShakePattern> ShakePattern;

public:
	void SetLocationOscillator(const FWaveOscillator& Oscillator, float InLocationAmplitudeMultiplier, float InLocationFrequencyMultiplier) const;

	void SetRollOscillator(const FWaveOscillator& Oscillator, float InRotationAmplitudeMultiplier, float InRotationFrequencyMultiplier) const;

	FWaveOscillator GetOscillator() const;

	FWaveOscillator GetRollOscillator() const;

	float GetLocationAmplitudeMultiplier() const;

	float GetRotationAmplitudeMultiplier() const;

	float GetLocationFrequencyMultiplier() const;

	static FWaveOscillator GetEmptyOscillator();
};
