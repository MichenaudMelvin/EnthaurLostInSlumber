// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/PRFPressAny.h"

#include "PRFUIManager.h"

void UPRFPressAny::NativeConstruct()
{
	Super::NativeConstruct();

	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	UIManager->SetUIInputMode();
	//UIManager->SetMenuState(EPRFUIState::AnyMenu);
}
