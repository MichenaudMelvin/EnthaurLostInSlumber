// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/ENTWidgetBasics.h"
#include "Menus/Elements/ENTCustomCheckBox.h"
#include "Menus/Elements/ENTCustomSlider.h"
#include "ENTOptionsMenu.generated.h"

class UVerticalBox;
class UTextBlock;

UCLASS()
class ENTUI_API UENTOptionsMenu : public UENTWidgetBasics
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void BeginDestroy() override;

	void UpdateWidgetValues(bool bSkipAnim);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> OverallVolumeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> MouseSensitivityButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomSlider> OverallVolumeSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomSlider> MouseSensitivitySlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> InvertMouseAxisButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomCheckBox> InvertMouseAxisCheckBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> ViewBobbingButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomCheckBox> ViewBobbingCheckbox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> ViewControlsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UENTCustomButton> ResetButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> InformationVerticalBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> OptionTitle;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
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
	void OnViewBobbingCheckBoxClicked(bool bIsChecked, bool bSkip);

	UFUNCTION()
	void OnMouseYAxisCheckBoxClicked(bool bIsChecked, bool bSkip);

	UFUNCTION()
	void OnMouseSensitivitySliderChanged(float InValue);

	UFUNCTION()
	void ResetSettings();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> OverallVolumeValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MouseSensitivityValue;
};
