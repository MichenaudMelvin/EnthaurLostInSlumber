// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "PRFCustomSlider.generated.h"

/**
 * 
 */
UCLASS()
class PRFUI_API UPRFCustomSlider : public UUserWidget
{
	GENERATED_BODY()

public:
	TObjectPtr<USlider> GetCustomSlider() const { return CustomSlider; }
	TObjectPtr<UProgressBar> GetBackgroundProgressBar() const { return ProgressBarBackground; }

protected:
	virtual void NativeOnInitialized() override;
	virtual void BeginDestroy() override;

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<USlider> CustomSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBarBackground;

	UFUNCTION()
	void OnCustomSliderValueChanged(float InValue);
};
