// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "Menus/PRFWidgetBasics.h"
#include "PRFPauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class PRFUI_API UPRFPauseMenu : public UPRFWidgetBasics
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleOptionsMenuButton();

	UFUNCTION()
	void HandleRestartCheckpointButton();

	UFUNCTION()
	void HandleMainMenuButton();

	UFUNCTION()
	void HandleQuitButton();

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> OptionsMenuButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> RestartCheckpointButton;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> MainMenuButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> QuitButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> AreaName;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> RegionName;
};
