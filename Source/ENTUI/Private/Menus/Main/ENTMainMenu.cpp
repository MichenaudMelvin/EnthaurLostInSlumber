// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/ENTMainMenu.h"
#include "Subsystems/ENTMenuManager.h"
#include "Config/ENTUIConfig.h"
#include "Components/Button.h"
#include "Subsystems/ENTPlayerSaveSubsystem.h"

void UENTMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (NewGameButton && NewGameButton->GetCustomButton())
	{
		NewGameButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTMainMenu::HandleNewGameMenu);
	}
	if (ContinueButton && ContinueButton->GetCustomButton())
	{
		ContinueButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTMainMenu::HandleContinueInteraction);
	}
	if (OptionsButton && OptionsButton->GetCustomButton())
	{
		OptionsButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTMainMenu::HandleOptionsMenu);
	}
	if (CreditsButton && CreditsButton->GetCustomButton())
	{
		CreditsButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTMainMenu::HandleCreditsMenu);
	}
	if (QuitButton && QuitButton->GetCustomButton())
	{
		QuitButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTMainMenu::HandleQuitMenu);
	}
}

void UENTMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UENTPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UENTPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem)
	{
		return;
	}

	if (!PlayerSaveSubsystem->DoesSaveGameExist(0))
	{
		ContinueButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		ContinueButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void UENTMainMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (NewGameButton && NewGameButton->GetCustomButton())
	{
		NewGameButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTMainMenu::HandleNewGameMenu);
	}
	if (ContinueButton && ContinueButton->GetCustomButton())
	{
		ContinueButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTMainMenu::HandleContinueInteraction);
	}
	if (OptionsButton && OptionsButton->GetCustomButton())
	{
		OptionsButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTMainMenu::HandleOptionsMenu);
	}
	if (CreditsButton && CreditsButton->GetCustomButton())
	{
		CreditsButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTMainMenu::HandleCreditsMenu);
	}
	if (QuitButton && QuitButton->GetCustomButton())
	{
		QuitButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTMainMenu::HandleQuitMenu);
	}
}

void UENTMainMenu::HandleNewGameMenu()
{
	if (!IsValid(GetUIConfig()->NewGameMenuClass))
	{
		return;
	}

	UENTPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UENTPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem)
	{
		return;
	}

	if (PlayerSaveSubsystem->DoesSaveGameExist(0))
	{
		GetMenuManager()->OpenMenu(GetMenuManager()->GetNewGameMenu(), false);
	}
	else
	{
		PlayerSaveSubsystem->StartNewGame();

		GetMenuManager()->SetMenuState(EENTMenuState::Waiting);
		GetMenuManager()->CloseAllMenus(EENTMenuState::Gameplay);
	}
}

void UENTMainMenu::HandleContinueInteraction()
{
	UENTPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UENTPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem || !IsValid(GetMenuManager()))
	{
		return;
	}

	GetMenuManager()->SetMenuState(EENTMenuState::Waiting);
	GetMenuManager()->CloseAllMenus(EENTMenuState::Gameplay);

	PlayerSaveSubsystem->ContinueGame();
}

void UENTMainMenu::HandleOptionsMenu()
{
	if (!IsValid(GetUIConfig()->OptionsMenuClass))
	{
		return;
	}

	GetMenuManager()->OpenMenu(GetMenuManager()->GetOptionsMenu(), false);
}

void UENTMainMenu::HandleCreditsMenu()
{
	if (!IsValid(GetUIConfig()->CreditsMenuClass))
	{
		return;
	}

	GetMenuManager()->OpenMenu(GetMenuManager()->GetCreditsMenu(), false);
}

void UENTMainMenu::HandleQuitMenu()
{
	if (!IsValid(GetUIConfig()->QuitMenuClass))
	{
		return;
	}

	GetMenuManager()->OpenMenu(GetMenuManager()->GetQuitMenu(), false);
}

const UENTUIConfig* UENTMainMenu::GetUIConfig() const
{
	return GetDefault<UENTUIConfig>();
}

UENTMenuManager* UENTMainMenu::GetMenuManager() const
{
	return GetGameInstance()->GetSubsystem<UENTMenuManager>();
}
