// Fill out your copyright notice in the Description page of Project Settings.


#include "Kevin/UI/MainMenuUI.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainMenuUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	PlayButton->OnClicked.AddDynamic(this, &UMainMenuUI::StartGame);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenuUI::QuitGame);
}

void UMainMenuUI::StartGame()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(
		this,
		LevelToLoad);
}

void UMainMenuUI::QuitGame()
{
	UKismetSystemLibrary::QuitGame(
		this,
		UGameplayStatics::GetPlayerController(this, 0),
		EQuitPreference::Quit,
		true);
}

