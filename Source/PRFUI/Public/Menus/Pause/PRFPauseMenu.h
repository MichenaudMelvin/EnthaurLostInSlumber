// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
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
	void HandleMainMenuButton();

	UFUNCTION()
	void HandleOptionsMenuButton();

	UFUNCTION()
	void HandleRestartCheckpointButton();

	UPROPERTY(Meta = (BindWidget))
	UButton* MainMenuButton;
	
	UPROPERTY(Meta = (BindWidget))
	UButton* OptionsMenuButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* RestartCheckpointButton;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> MainMenuLevel;
};
