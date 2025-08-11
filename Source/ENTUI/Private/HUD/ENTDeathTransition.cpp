// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ENTDeathTransition.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ENTDefaultCharacter.h"

void UENTDeathTransition::RespawnPlayer()
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Character)
	{
		return;
	}

	AENTDefaultCharacter* Player = Cast<AENTDefaultCharacter>(Character);
	if (!Player)
	{
		return;
	}

	Player->Respawn();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController)
	{
		return;
	}

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController, true);
	PlayerController->SetShowMouseCursor(false);
}
