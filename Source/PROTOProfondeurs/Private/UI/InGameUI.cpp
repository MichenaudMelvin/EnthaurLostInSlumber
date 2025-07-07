// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameUI.h"
#include "FCTween.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"

void UInGameUI::NativeConstruct()
{
	Super::NativeConstruct();

	Player = Cast<AFirstPersonCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (Player)
	{
		Player->OnAmberUpdate.AddDynamic(this, &UInGameUI::OnAmberUpdate);
	}
}

void UInGameUI::OnAmberUpdate(EAmberType AmberType, int AmberAmount)
{
	if (AmberType == EAmberType::WeakAmber)
	{
		AmberAmount > 0 ? OnAmberPickUp.Broadcast() : OnAmberUsed.Broadcast();
	}
}

void UInGameUI::SetPropulsionActive(bool active)
{
	PlayNerveActionReadyDelegate.Broadcast(active);
}

void UInGameUI::SetInteraction(const bool bActive) const
{
	PlayInteractionReadyDelegate.Broadcast(bActive);
}