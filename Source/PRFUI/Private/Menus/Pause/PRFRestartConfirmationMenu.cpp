// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Pause/PRFRestartConfirmationMenu.h"
#include "PRFUIManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"

void UPRFRestartConfirmationMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton && RestartButton->GetCustomButton())
	{
		RestartButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFRestartConfirmationMenu::HandleRestartAction);
	}
}

void UPRFRestartConfirmationMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (RestartButton && RestartButton->GetCustomButton())
	{
		RestartButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFRestartConfirmationMenu::HandleRestartAction);
	}
}

void UPRFRestartConfirmationMenu::HandleRestartAction()
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Character)
	{
		return;
	}

	AFirstPersonCharacter* Player = Cast<AFirstPersonCharacter>(Character);
	if (!Player)
	{
		return;
	}

	Player->Respawn();

	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	UIManager->CloseAllMenus(EPRFUIState::Gameplay);
}
