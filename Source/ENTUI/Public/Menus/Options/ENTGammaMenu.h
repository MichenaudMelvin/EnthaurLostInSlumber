// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Menus/ENTWidgetBasics.h"
#include "Menus/Elements/ENTCustomSlider.h"
#include "ENTGammaMenu.generated.h"

/**
 * 
 */
UCLASS()
class ENTUI_API UENTGammaMenu : public UENTWidgetBasics
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;
	
	UFUNCTION()
	void OnGammaSliderValueChanged(float InValue);
	
	void UpdateWidgetValues();
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> GammaImage;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomSlider> GammaSlider;
};
