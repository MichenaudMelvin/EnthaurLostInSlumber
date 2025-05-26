// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
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
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;

	UPROPERTY(Meta = (BindWidget))
	UButton* ControlsButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* DisplayButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* GraphicsButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* AudioButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* AccessibilityButton;
};
