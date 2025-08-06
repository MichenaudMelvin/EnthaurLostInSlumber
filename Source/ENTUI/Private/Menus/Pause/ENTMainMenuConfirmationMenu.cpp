// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Pause/ENTMainMenuConfirmationMenu.h"
#include "Subsystems/ENTMenuManager.h"
#include "Kismet/GameplayStatics.h"
#include "Menus/Elements/ENTCustomButton.h"

void UENTMainMenuConfirmationMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (MainMenuButton && MainMenuButton->GetCustomButton())
	{
		MainMenuButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTMainMenuConfirmationMenu::HandleMainMenuAction);
	}
}

void UENTMainMenuConfirmationMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (MainMenuButton && MainMenuButton->GetCustomButton())
	{
		MainMenuButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTMainMenuConfirmationMenu::HandleMainMenuAction);
	}
}

void UENTMainMenuConfirmationMenu::HandleMainMenuAction()
{
	UENTMenuManager* UIManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	UIManager->CloseAllMenus(EENTMenuState::AnyMenu);
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainMenuLevel);
}
