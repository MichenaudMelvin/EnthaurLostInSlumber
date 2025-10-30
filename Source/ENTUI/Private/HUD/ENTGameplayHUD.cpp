// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ENTGameplayHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ENTDefaultCharacter.h"

void UENTGameplayHUD::NativeConstruct()
{
	Super::NativeConstruct();

	RebindDelegates();
}

void UENTGameplayHUD::OnAmberUpdate(EAmberType AmberType, int InAmberAmount)
{
	if (AmberType == EAmberType::WeakAmber)
	{
		if (AmberAmount != InAmberAmount)
		{
			AmberAmount = InAmberAmount;
			AmberAmount > 0 ? OnAmberPickUp.Broadcast() : OnAmberUsed.Broadcast();
		}
	}
}

void UENTGameplayHUD::RebindDelegates()
{
	if (!Player)
	{
		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (!PlayerCharacter)
		{
			return;
		}

		Player = Cast<AENTDefaultCharacter>(PlayerCharacter);
	}

	if (Player)
	{
		if (!Player->OnAmberUpdate.IsAlreadyBound(this, &UENTGameplayHUD::OnAmberUpdate))
		{
			Player->OnAmberUpdate.AddDynamic(this, &UENTGameplayHUD::OnAmberUpdate);
		}

		if (!Player->OnInteractionFeedback.IsAlreadyBound(this, &UENTGameplayHUD::SetInteraction))
		{
			Player->OnInteractionFeedback.AddDynamic(this, &UENTGameplayHUD::SetInteraction);
		}
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