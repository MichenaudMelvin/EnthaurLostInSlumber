// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/ENTPressAny.h"
#include "Subsystems/ENTMenuManager.h"

void UENTPressAny::NativeConstruct()
{
	Super::NativeConstruct();

	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}

	MenuManager->SetUIInputMode();
	//MenuManager->SetMenuState(EPRFUIState::AnyMenu);
}
