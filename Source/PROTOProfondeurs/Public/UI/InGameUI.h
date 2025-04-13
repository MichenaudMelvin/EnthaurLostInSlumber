// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameElements/AmberOre.h"
#include "InGameUI.generated.h"

class FCTweenInstance;
class UImage;
/**
 * 
 */
UCLASS()
class PROTOPROFONDEURS_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Crosshair;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Interact;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PropulsionIndicator;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> NecrosisGauge1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> NecrosisGauge2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> NecrosisGauge3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> GrayAmberGauge;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> GrayAmberGauge_Enabled;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> NecrosisAmberGauge_Step1;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> NecrosisAmberGauge_Step2;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> NecrosisAmberGauge_Step3;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> NecrosisAmberGauge_Used;

	UPROPERTY()
	TObjectPtr<class AFirstPersonCharacter> Player;

	virtual void NativeConstruct() override;

	void SetPropulsionActive(bool active);
	void SetInteraction(bool bActive) const;

private:
	FCTweenInstance* CurrentTween;
	TObjectPtr<UImage> Img;

protected:
	UFUNCTION()
	void AmberChargeChanged(EAmberType AmberType, int AmberAmount);
	virtual void NativeOnInitialized() override;
};
