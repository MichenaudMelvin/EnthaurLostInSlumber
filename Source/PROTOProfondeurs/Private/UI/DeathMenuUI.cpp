// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DeathMenuUI.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "GameElements/RespawnTree.h"
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
	auto Player = Cast<AFirstPersonCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!Player)
	{
		return;
	}

	ARespawnTree* RespawnTree = Player->GetRespawnTree();
	if (!RespawnTree)
	{
		return;
	}

	Player->SetActorLocation(RespawnTree->GetActorLocation());

	auto PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return;
	}

	PC->SetPause(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PC, true);
	PC->SetShowMouseCursor(false);

	this->RemoveFromParent();
}

void UDeathMenuUI::BackToMainMenu()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(
		this,
		MainMenuLevel);
}
