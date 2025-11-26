// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "Menus/ENTWidgetBasics.h"
#include "Menus/Elements/ENTCustomCheckBox.h"
#include "Menus/Elements/ENTCustomSlider.h"
#include "ENTAccessibilityMenu.generated.h"

/**
 * 
 */
UCLASS()
class ENTUI_API UENTAccessibilityMenu : public UENTWidgetBasics
{
	GENERATED_BODY()
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;

protected:
	UFUNCTION()
	void OnMouseSensitivitySliderChanged(float InValue);

	UFUNCTION()
	void OnMouseSensitivityButtonHovered();

	UFUNCTION()
	void OnInvertMouseAxisButtonHovered();

	UFUNCTION()
	void OnViewBobbingButtonHovered();

	UFUNCTION()
	void OnMouseYAxisCheckBoxClicked(bool bIsChecked, bool bSkip);

	UFUNCTION()
	void OnViewBobbingCheckBoxClicked(bool bIsChecked, bool bSkip);

	UFUNCTION()
	void OpenResetSettingsMenu();
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> MouseSensitivityButton;

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
	TObjectPtr<UTextBlock> MouseSensitivityValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> OptionTitle;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> OptionDescription;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> ResetButton;
};
