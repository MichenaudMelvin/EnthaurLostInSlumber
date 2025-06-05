// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/PRFNewGameMenu.h"
#include "Components/Button.h"
#include "Saves/PlayerSaveSubsystem.h"

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
	UPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem)
	{
		return;
	}

	PlayerSaveSubsystem->StartNewGame();
}
