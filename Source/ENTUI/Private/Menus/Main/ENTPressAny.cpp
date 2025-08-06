// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/ENTPressAny.h"
#include "Subsystems/ENTMenuManager.h"

void UENTPressAny::NativeConstruct()
{
	Super::NativeConstruct();

	UENTMenuManager* UIManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	UIManager->SetUIInputMode();
	//UIManager->SetMenuState(EPRFUIState::AnyMenu);
}
