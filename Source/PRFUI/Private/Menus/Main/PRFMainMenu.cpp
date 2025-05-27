// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/PRFMainMenu.h"

#include "PRFUIManager.h"
#include "UIManagerSettings.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UPRFMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (NewGameButton)
	{
		NewGameButton->OnClicked.AddDynamic(this, &UPRFMainMenu::HandleNewGameMenu);
	}
	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UPRFMainMenu::HandleContinueInteraction);
	}
	if (OptionsButton)
	{
		OptionsButton->OnClicked.AddDynamic(this, &UPRFMainMenu::HandleOptionsMenu);
	}
	if (CreditsButton)
	{
		CreditsButton->OnClicked.AddDynamic(this, &UPRFMainMenu::HandleCreditsMenu);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UPRFMainMenu::HandleQuitMenu);
	}
}

void UPRFMainMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (NewGameButton)
	{
		NewGameButton->OnClicked.RemoveDynamic(this, &UPRFMainMenu::HandleNewGameMenu);
	}
	if (ContinueButton)
	{
		ContinueButton->OnClicked.RemoveDynamic(this, &UPRFMainMenu::HandleContinueInteraction);
	}
	if (OptionsButton)
	{
		OptionsButton->OnClicked.RemoveDynamic(this, &UPRFMainMenu::HandleOptionsMenu);
	}
	if (CreditsButton)
	{
		CreditsButton->OnClicked.RemoveDynamic(this, &UPRFMainMenu::HandleCreditsMenu);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.RemoveDynamic(this, &UPRFMainMenu::HandleQuitMenu);
	}
}

void UPRFMainMenu::HandleNewGameMenu()
{
	if (!IsValid(GetUIManagerSettings()->NewGameMenuClass))
	{
		return;
	}

	if (nullptr)
	{
		/* Todo Melvin Check s'il y a déjà une savegame qui existe
		/ Si oui ça envoie en dessous pour ouvrir le menu qui prévient
		/ Si non, tu lance direct la game
		*/
	}
	else
	{
		GetUIManager()->OpenMenu(GetUIManager()->GetNewGameMenu(), false);
	}
}

void UPRFMainMenu::HandleContinueInteraction()
{
	// Todo Melvin - Load save & continue game

	// TEMPORARY:
	

	if (!IsValid(GetUIManager()))
	{
		return;
	}

	GetUIManager()->SetMenuState(EPRFUIState::Waiting);
	GetUIManager()->CloseAllMenus(EPRFUIState::Gameplay);

	UGameplayStatics::OpenLevelBySoftObjectPtr(this, TempGameplayLevel);
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

const UUIManagerSettings* UPRFMainMenu::GetUIManagerSettings()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return nullptr;
	}

	const UUIManagerSettings* UIManagerSettings = GetDefault<UUIManagerSettings>();
	if (!IsValid(UIManagerSettings))
	{
		return nullptr;
	}

	return UIManagerSettings;
}

UPRFUIManager* UPRFMainMenu::GetUIManager()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return nullptr;
	}

	return UIManager;
}
