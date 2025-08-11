// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ENTGameplayHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ENTDefaultCharacter.h"

void UENTGameplayHUD::NativeConstruct()
{
	Super::NativeConstruct();

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!PlayerCharacter)
	{
		return;
	}

	Player = Cast<AENTDefaultCharacter>(PlayerCharacter);
	if (Player)
	{
		Player->OnAmberUpdate.AddDynamic(this, &UENTGameplayHUD::OnAmberUpdate);
	}
}

void UENTGameplayHUD::OnAmberUpdate(EAmberType AmberType, int AmberAmount)
{
	if (AmberType == EAmberType::WeakAmber)
	{
		AmberAmount > 0 ? OnAmberPickUp.Broadcast() : OnAmberUsed.Broadcast();
	}
}

void UENTGameplayHUD::SetPropulsionActive(bool active)
{
	PlayNerveActionReadyDelegate.Broadcast(active);
}

void UENTGameplayHUD::SetInteraction(bool bActive)
{
	PlayInteractionReadyDelegate.Broadcast(bActive);
}