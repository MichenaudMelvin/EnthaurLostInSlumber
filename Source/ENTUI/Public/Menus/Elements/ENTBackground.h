// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Menus/ENTWidgetBasics.h"
#include "ENTBackground.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBackgroundFadeAnimationFinished, UUserWidget*, Menu);

/**
 * 
 */
UCLASS()
class ENTUI_API UENTBackground : public UENTWidgetBasics
{
	GENERATED_BODY()

public:
	// UPROPERTY(BlueprintAssignable, BlueprintCallable)
	// FOnBackgroundFadeAnimationFinished OnBackgroundFadeAnimationFinished;
	//
	// UFUNCTION(BlueprintCallable)
	// void PlayFadeAnimation(bool bIsReverse);

protected:
	// UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
	// TObjectPtr<UWidgetAnimation> FadeAnimation;
	//
	// FWidgetAnimationStatePlayParams PlayParams;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;
};
