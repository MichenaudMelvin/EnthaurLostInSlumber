// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/PRFWidgetBasics.h"
#include "PRFNewGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class PRFUI_API UPRFNewGameMenu : public UPRFWidgetBasics
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> ContinueButton;

	virtual void NativeOnInitialized() override;
	virtual void BeginDestroy() override;

	UFUNCTION()
	void HandleNewGameInteraction();
};
