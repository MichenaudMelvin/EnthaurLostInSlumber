// Fill out your copyright notice in the Description page of Project Settings.


#include "PRFUIManager.h"

#include "PRFOptionsMenuState.h"
#include "UIManagerSettings.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void UPRFUIManager::ShowPauseMenu(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return;
	}

	if (OptionsMenuState == EPRFOptionsMenuState::InGame)
	{
		PauseMenu = CreateWidget<UUserWidget>(PlayerController, GetDefault<UUIManagerSettings>()->PauseMenuClass);
		if (!IsValid(PauseMenu))
		{
			return;
		}
		
		PauseMenu->AddToViewport();
		SetUIInputMode();
		OptionsMenuState = EPRFOptionsMenuState::InPause;
	}
	else if (OptionsMenuState == EPRFOptionsMenuState::InPause)
	{
		PauseMenu->RemoveFromParent();
		SetGameInputMode();
		OptionsMenuState = EPRFOptionsMenuState::InGame;
	}
	else if (OptionsMenuState == EPRFOptionsMenuState::InOptions)
	{
		
	}
}

void UPRFUIManager::SetUIInputMode() const
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!IsValid(World))
	{
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		return;
	}

	//PlayerController->SetInputMode(FInputModeGameAndUI());
	UGameplayStatics::SetGamePaused(this, true);
	PlayerController->bShowMouseCursor = true;
}

void UPRFUIManager::SetGameInputMode() const
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!IsValid(World))
	{
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		return;
	}

	//PlayerController->SetInputMode(FInputModeGameOnly());

	UGameplayStatics::SetGamePaused(this, false);
	PlayerController->bShowMouseCursor = false;
}
