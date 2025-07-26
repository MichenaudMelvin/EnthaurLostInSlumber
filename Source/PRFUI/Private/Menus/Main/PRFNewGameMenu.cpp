// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/PRFNewGameMenu.h"
#include "PRFUIManager.h"
#include "Components/Button.h"
#include "Subsystems/ENTPlayerSaveSubsystem.h"

void UPRFNewGameMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ContinueButton && ContinueButton->GetCustomButton())
	{
		ContinueButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFNewGameMenu::HandleNewGameInteraction);
	}
}

void UPRFNewGameMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (ContinueButton && ContinueButton->GetCustomButton())
	{
		ContinueButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFNewGameMenu::HandleNewGameInteraction);
	}
}

void UPRFNewGameMenu::HandleNewGameInteraction()
{
	UENTPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UENTPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem)
	{
		return;
	}

	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	PlayerSaveSubsystem->StartNewGame();

	UIManager->SetMenuState(EPRFUIState::Waiting);
	UIManager->CloseAllMenus(EPRFUIState::Gameplay);
}
