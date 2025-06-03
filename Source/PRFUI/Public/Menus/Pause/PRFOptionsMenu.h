// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Menus/PRFWidgetBasics.h"
#include "PRFOptionsMenu.generated.h"

/**
 * 
 */
UCLASS()
class PRFUI_API UPRFOptionsMenu : public UPRFWidgetBasics
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void BeginDestroy() override;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> OverallVolumeButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> MouseSensitivityButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USlider> OverallVolumeSlider;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USlider> MouseSensitivitySlider;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> InvertMouseAxisButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCheckBox> InvertMouseAxisCheckBox;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> ViewBobbingButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCheckBox> ViewBobbingCheckbox;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> ViewControlsButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UVerticalBox> InformationVerticalBox;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> OptionTitle;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> OptionDescription;

	UFUNCTION()
	void OnOverallButtonHovered();
	
	UFUNCTION()
	void OnMouseSensButtonHovered();

	UFUNCTION()
	void OnMouseInvertButtonHovered();

	UFUNCTION()
	void OnViewBobbingButtonHovered();

	UFUNCTION()
	void OnViewControlsButtonHovered();

	UFUNCTION()
	void OnViewControlsButtonClicked();

	UFUNCTION()
	void OnOverallSliderChanged(float InValue);

	UFUNCTION()
	void OnMouseSensSliderChanged();

	UFUNCTION()
	void OnViewBobbingCheckBoxClicked(bool bIsChecked);

	UFUNCTION()
	void OnMouseYAxisCheckBoxClicked(bool bIsChecked);

	UFUNCTION()

	void OnMouseSensitivitySliderChanged(float InValue);
};
