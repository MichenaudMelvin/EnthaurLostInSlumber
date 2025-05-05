// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/PRFMainMenu.h"

#include "PRFUIManager.h"
#include "UIManagerSettings.h"
#include "Components/Button.h"

void UPRFMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (NewGameButton)
	{
		NewGameButton->OnClicked.AddDynamic(this, &UPRFMainMenu::HandleNewGameMenu);
	}
	if (LoadGameButton)
	{
		NewGameButton->OnClicked.AddDynamic(this, &UPRFMainMenu::HandleLoadGameMenu);
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
	if (LoadGameButton)
	{
		NewGameButton->OnClicked.RemoveDynamic(this, &UPRFMainMenu::HandleLoadGameMenu);
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
	
	UUserWidget* NewGameMenu = CreateWidget<UUserWidget>(GetWorld(), GetUIManagerSettings()->NewGameMenuClass);
	GetUIManager()->OpenMenu(NewGameMenu, false);
}

void UPRFMainMenu::HandleLoadGameMenu()
{
	if (!IsValid(GetUIManagerSettings()->LoadGameMenuClass))
	{
		return;
	}
	
	UUserWidget* LoadGameMenu = CreateWidget<UUserWidget>(GetWorld(), GetUIManagerSettings()->LoadGameMenuClass);
	GetUIManager()->OpenMenu(LoadGameMenu, false);
}

void UPRFMainMenu::HandleOptionsMenu()
{
	if (!IsValid(GetUIManagerSettings()->OptionsMenuClass))
	{
		return;
	}

	UUserWidget* OptionsMenu = CreateWidget<UUserWidget>(GetWorld(), GetUIManagerSettings()->OptionsMenuClass);
	GetUIManager()->OpenMenu(OptionsMenu, false);
}

void UPRFMainMenu::HandleCreditsMenu()
{
	if (!IsValid(GetUIManagerSettings()->CreditsMenuClass))
	{
		return;
	}

	UUserWidget* CreditsMenu = CreateWidget<UUserWidget>(GetWorld(), GetUIManagerSettings()->CreditsMenuClass);
	GetUIManager()->OpenMenu(CreditsMenu, false);
}

void UPRFMainMenu::HandleQuitMenu()
{
	if (!IsValid(GetUIManagerSettings()->QuitMenuClass))
	{
		return;
	}

	UUserWidget* QuitMenu = CreateWidget<UUserWidget>(GetWorld(), GetUIManagerSettings()->QuitMenuClass);
	GetUIManager()->OpenMenu(QuitMenu, false);
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
