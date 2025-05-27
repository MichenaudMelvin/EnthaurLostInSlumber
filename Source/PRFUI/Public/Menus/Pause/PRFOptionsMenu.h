// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	virtual void BeginDestroy() override;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> OverallVolumeButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> MouseSensitivityButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> InvertMouseAxisButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> ViewBobbingButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> ViewControlsButton;

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
	void OnViewControlsButton();
};
