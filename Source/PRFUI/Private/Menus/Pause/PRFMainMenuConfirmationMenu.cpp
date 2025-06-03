// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Pause/PRFMainMenuConfirmationMenu.h"

#include "PRFUIManager.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UPRFMainMenuConfirmationMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (MainMenuButton && MainMenuButton->GetCustomButton())
	{
		MainMenuButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFMainMenuConfirmationMenu::HandleMainMenuAction);
	}
}

void UPRFMainMenuConfirmationMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (MainMenuButton && MainMenuButton->GetCustomButton())
	{
		MainMenuButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFMainMenuConfirmationMenu::HandleMainMenuAction);
	}
}

void UPRFMainMenuConfirmationMenu::HandleMainMenuAction()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}
	
	UIManager->CloseAllMenus(EPRFUIState::AnyMenu);
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainMenuLevel);
}
