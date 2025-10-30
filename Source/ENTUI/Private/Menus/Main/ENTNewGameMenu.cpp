// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/ENTNewGameMenu.h"
#include "Menus/Elements/ENTCustomButton.h"
#include "Subsystems/ENTMenuManager.h"
#include "Subsystems/ENTPlayerSaveSubsystem.h"

void UENTNewGameMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ContinueButton && ContinueButton->GetCustomButton())
	{
		ContinueButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTNewGameMenu::HandleNewGameInteraction);
	}
}

void UENTNewGameMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (ContinueButton && ContinueButton->GetCustomButton())
	{
		ContinueButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTNewGameMenu::HandleNewGameInteraction);
	}
}

void UENTNewGameMenu::HandleNewGameInteraction()
{
	UENTPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UENTPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem)
	{
		return;
	}

	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}

	PlayerSaveSubsystem->StartNewGame();

	MenuManager->SetMenuState(EENTMenuState::Waiting);
	MenuManager->CloseAllMenus(EENTMenuState::Gameplay);
}
