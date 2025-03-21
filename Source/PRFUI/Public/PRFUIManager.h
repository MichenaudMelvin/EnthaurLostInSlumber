// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PRFOptionsMenuState.h"
#include "PauseMenus/PRFPauseMenu.h"
#include "UObject/Object.h"
#include "PRFUIManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PRFUI_API UPRFUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void ShowPauseMenu(APlayerController* PlayerController);

protected:
	void SetUIInputMode() const;
	void SetGameInputMode() const;

	UPROPERTY()
	UUserWidget* PauseMenu = nullptr;

	EPRFOptionsMenuState OptionsMenuState = EPRFOptionsMenuState::InGame;
};
