// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Pause/ENTRestartConfirmationMenu.h"
#include "Subsystems/ENTMenuManager.h"
#include "Kismet/GameplayStatics.h"
#include "Menus/Elements/ENTCustomButton.h"
#include "Player/FirstPersonCharacter.h"

void UENTRestartConfirmationMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton && RestartButton->GetCustomButton())
	{
		RestartButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTRestartConfirmationMenu::HandleRestartAction);
	}
}

void UENTRestartConfirmationMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (RestartButton && RestartButton->GetCustomButton())
	{
		RestartButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTRestartConfirmationMenu::HandleRestartAction);
	}
}

void UENTRestartConfirmationMenu::HandleRestartAction()
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

	UENTMenuManager* UIManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	UIManager->CloseAllMenus(EENTMenuState::Gameplay);
}
