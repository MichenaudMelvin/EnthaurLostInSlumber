// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ENTWidgetBasics.generated.h"

class UENTCustomButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFadeAnimationFinished, UUserWidget*, Menu);

UCLASS(Abstract)
class ENTUI_API UENTWidgetBasics : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnFadeAnimationFinished OnFadeAnimationFinishedHandle;
	
	UFUNCTION(BlueprintCallable)
	void PlayFadeAnimation(bool bIsReverse);
	
protected:
	virtual void NativeOnInitialized() override;

	virtual void BeginDestroy() override;
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> FadeAnimation;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UENTCustomButton> BackButton;

	UFUNCTION()
	void RemoveMenuAfterAnim();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HandleBackAction();

	FWidgetAnimationStatePlayParams PlayParams;
};
