// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PRFPauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class PRFUI_API UPRFPauseMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	static void TogglePauseMenu();

protected:
	virtual void NativeOnInitialized() override;

	/*UPROPERTY(Meta = (BindWidget))
	UTextBlock* TestBlock;*/
};
