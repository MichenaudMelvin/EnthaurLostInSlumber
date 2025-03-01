// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	TObjectPtr<UImage> PropulsionIndicator;

	virtual void NativeConstruct() override;

	void SetPropulsionActive(bool active);

private:
	FCTweenInstance* CurrentTween;
};
