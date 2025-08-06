// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "ENTCustomSlider.generated.h"

UCLASS()
class ENTUI_API UENTCustomSlider : public UUserWidget
{
	GENERATED_BODY()

public:
	TObjectPtr<USlider> GetCustomSlider() const { return CustomSlider; }
	TObjectPtr<UProgressBar> GetBackgroundProgressBar() const { return ProgressBarBackground; }

protected:
	virtual void NativeOnInitialized() override;

	virtual void BeginDestroy() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USlider> CustomSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBarBackground;

	UFUNCTION()
	void OnCustomSliderValueChanged(float InValue);
};
