// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/PRFWidgetBasics.h"
#include "PRFCreditsMenu.generated.h"

/**
 * 
 */
UCLASS()
class PRFUI_API UPRFCreditsMenu : public UPRFWidgetBasics
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> NamesAnimation;
};
