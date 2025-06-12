// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameElements/AmberOre.h"
#include "InGameUI.generated.h"

class FCTweenInstance;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNerveActionReady, bool, bValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionReady, bool, bValue);

/**
 * 
 */
UCLASS()
class PROTOPROFONDEURS_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnAmberUpdate(EAmberType AmberType, int AmberAmount);
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Crosshair;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Interact;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PropulsionIndicator;

	UPROPERTY()
	TObjectPtr<class AFirstPersonCharacter> Player;

	void SetPropulsionActive(bool active);
	void SetInteraction(bool bActive) const;

protected:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmberPickUp);
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAmberPickUp OnAmberPickUp;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmberUsed);
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAmberUsed OnAmberUsed;

	UPROPERTY(meta = (BindWidgetAnim), Transient, BlueprintReadOnly)
	TObjectPtr<UWidgetAnimation> NerveActionReadyAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient, BlueprintReadOnly)
	TObjectPtr<UWidgetAnimation> InteractionReadyAnimation;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnNerveActionReady PlayNerveActionReadyDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnInteractionReady PlayInteractionReadyDelegate;

private:
	FCTweenInstance* CurrentTween;
	TObjectPtr<UImage> Img;
};
