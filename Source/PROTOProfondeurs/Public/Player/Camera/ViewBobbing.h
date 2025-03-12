// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "ViewBobbing.generated.h"

class UWaveOscillatorCameraShakePattern;
struct FWaveOscillator;

UCLASS()
class PROTOPROFONDEURS_API UViewBobbing : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UViewBobbing(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY()
	TObjectPtr<UWaveOscillatorCameraShakePattern> ShakePattern;

public:
	void SetOscillator(const FWaveOscillator& Oscillator) const;

	FWaveOscillator GetOscillator() const;

	static FWaveOscillator GetEmptyOscillator();
};
