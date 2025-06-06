// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/PRFMainMenu.h"
#include "PRFUIManager.h"
#include "UIManagerSettings.h"
#include "Components/Button.h"
#include "Saves/PlayerSaveSubsystem.h"

void UPRFMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (NewGameButton && NewGameButton->GetCustomButton())
	{
		NewGameButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFMainMenu::HandleNewGameMenu);
	}
	if (ContinueButton && ContinueButton->GetCustomButton())
	{
		ContinueButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFMainMenu::HandleContinueInteraction);
	}
	if (OptionsButton && OptionsButton->GetCustomButton())
	{
		OptionsButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFMainMenu::HandleOptionsMenu);
	}
	if (CreditsButton && CreditsButton->GetCustomButton())
	{
		CreditsButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFMainMenu::HandleCreditsMenu);
	}
	if (QuitButton && QuitButton->GetCustomButton())
	{
		QuitButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFMainMenu::HandleQuitMenu);
	}
}

void UPRFMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UPlayerSaveSubsystem>();
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

void UPRFMainMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (NewGameButton && NewGameButton->GetCustomButton())
	{
		NewGameButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFMainMenu::HandleNewGameMenu);
	}
	if (ContinueButton && ContinueButton->GetCustomButton())
	{
		ContinueButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFMainMenu::HandleContinueInteraction);
	}
	if (OptionsButton && OptionsButton->GetCustomButton())
	{
		OptionsButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFMainMenu::HandleOptionsMenu);
	}
	if (CreditsButton && CreditsButton->GetCustomButton())
	{
		CreditsButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFMainMenu::HandleCreditsMenu);
	}
	if (QuitButton && QuitButton->GetCustomButton())
	{
		QuitButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFMainMenu::HandleQuitMenu);
	}
}

void UPRFMainMenu::HandleNewGameMenu()
{
	if (!IsValid(GetUIManagerSettings()->NewGameMenuClass))
	{
		return;
	}

	UPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem)
	{
		return;
	}

	if (PlayerSaveSubsystem->DoesSaveGameExist(0))
	{
		GetUIManager()->OpenMenu(GetUIManager()->GetNewGameMenu(), false);
	}
	else
	{
		//GetUIManager()->SetMenuState(EPRFUIState::Waiting);
		//GetUIManager()->CloseAllMenus(EPRFUIState::Gameplay);

		PlayerSaveSubsystem->StartNewGame();
	}
}

void UPRFMainMenu::HandleContinueInteraction()
{
	UPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem || !IsValid(GetUIManager()))
	{
		return;
	}

	GetUIManager()->SetMenuState(EPRFUIState::Waiting);
	GetUIManager()->CloseAllMenus(EPRFUIState::Gameplay);

	PlayerSaveSubsystem->ContinueGame();
}

void UPRFMainMenu::HandleOptionsMenu()
{
	if (!IsValid(GetUIManagerSettings()->OptionsMenuClass))
	{
		return;
	}

	GetUIManager()->OpenMenu(GetUIManager()->GetOptionsMenu(), false);
}

void UPRFMainMenu::HandleCreditsMenu()
{
	if (!IsValid(GetUIManagerSettings()->CreditsMenuClass))
	{
		return;
	}

	GetUIManager()->OpenMenu(GetUIManager()->GetCreditsMenu(), false);
}

void UPRFMainMenu::HandleQuitMenu()
{
	if (!IsValid(GetUIManagerSettings()->QuitMenuClass))
	{
		return;
	}

	GetUIManager()->OpenMenu(GetUIManager()->GetQuitMenu(), false);
}

const UUIManagerSettings* UPRFMainMenu::GetUIManagerSettings() const
{
	return GetDefault<UUIManagerSettings>();
}

UPRFUIManager* UPRFMainMenu::GetUIManager() const
{
	return GetGameInstance()->GetSubsystem<UPRFUIManager>();
}
