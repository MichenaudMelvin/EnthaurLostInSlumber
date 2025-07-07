// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameElements/AmberOre.h"
#include "InGameUI.generated.h"

class FCTweenInstance;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmberUsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmberPickUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNerveActionReady, bool, bValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionReady, bool, bValue);

UCLASS()
class PROTOPROFONDEURS_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Animations", meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> NerveActionReadyAnimation;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Animations", meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> InteractionReadyAnimation;

	UPROPERTY()
	TObjectPtr<class AFirstPersonCharacter> Player;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Deletages")
	FOnAmberPickUp OnAmberPickUp;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Deletages")
	FOnAmberUsed OnAmberUsed;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Deletages")
	FOnNerveActionReady PlayNerveActionReadyDelegate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Deletages")
	FOnInteractionReady PlayInteractionReadyDelegate;

	UFUNCTION()
	void OnAmberUpdate(EAmberType AmberType, int AmberAmount);

public:
	void SetPropulsionActive(bool active);

	void SetInteraction(bool bActive) const;
};
