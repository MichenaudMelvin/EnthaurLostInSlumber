// Fill out your copyright notice in the Description page of Project Settings.


#include "Kevin/UI/DeathMenuUI.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"

void UDeathMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDeathMenuUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RespawnButton->OnClicked.AddDynamic(this, &UDeathMenuUI::RespawnPlayer);
	BackMainMenuButton->OnClicked.AddDynamic(this, &UDeathMenuUI::BackToMainMenu);
}

void UDeathMenuUI::RespawnPlayer()
{
	
	
	this->RemoveFromParent();
}

void UDeathMenuUI::BackToMainMenu()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(
		this,
		MainMenuLevel);
}
