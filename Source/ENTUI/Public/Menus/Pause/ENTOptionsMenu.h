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
	virtual void NativeDestruct() override;
	virtual void BeginDestroy() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> VolumeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> AccessibilityButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> ControlsButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> GammaButton;

	UFUNCTION()
	void OnVolumeButtonClicked();
	
	UFUNCTION()
	void OnViewControlsButtonClicked();
	
	UFUNCTION()
	void OnGammaButtonClicked();

	UFUNCTION()
	void OnAccessibilityButtonClicked();
};
