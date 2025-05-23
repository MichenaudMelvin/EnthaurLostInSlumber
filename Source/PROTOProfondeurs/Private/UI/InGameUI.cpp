// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameUI.h"
#include "FCTween.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"

void UInGameUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInGameUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Player = Cast<AFirstPersonCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (Player)
	{
		Player->OnAmberUpdate.AddDynamic(this, &UInGameUI::AmberChargeChanged);
	}
}

void UInGameUI::SetPropulsionActive(bool active)
{
	if (active)
	{
		FCTween::Play(
			0.f,
			1.f,
			[&](float x)
			{
				PropulsionIndicator->SetOpacity(x);
			},
			0.5f,
			EFCEase::OutSine)->SetOnComplete([&]
		{
			CurrentTween = FCTween::Play(
				1.f,
				0.f,
				[&](float x)
				{
					PropulsionIndicator->SetOpacity(x);
				},
				0.5f,
				EFCEase::OutSine)->SetYoyo(true)->SetLoops(-1);
		});
	}
	else
	{
		if (CurrentTween != nullptr)
		{
			CurrentTween->Destroy();
			FCTween::Play(
				1.f,
				0.f,
				[&](float x)
				{
					PropulsionIndicator->SetOpacity(x);
				},
				0.5f,
				EFCEase::OutSine);

			CurrentTween = nullptr;
		}
	}
}

void UInGameUI::SetInteraction(const bool bActive) const
{
	bActive ? Interact->SetOpacity(1.f) : Interact->SetOpacity(0.f);
}

void UInGameUI::AmberChargeChanged(EAmberType AmberType, int AmberAmount)
{
	switch (AmberType)
	{
	case EAmberType::NecroseAmber:
		{
			if (AmberAmount == 0)
			{
				PlayAnimationForward(NecrosisAmberGauge_Used);
			}
			else
			{
				if (AmberAmount == 1)
				{
					PlayAnimationForward(NecrosisAmberGauge_Step1);
				}
				else if (AmberAmount == 2)
				{
					PlayAnimationForward(NecrosisAmberGauge_Step2);
				}
				else if (AmberAmount == 3)
				{
					PlayAnimationForward(NecrosisAmberGauge_Step3);
				}
			}
			break;
		}

	case EAmberType::WeakAmber:
		{
			if (AmberAmount == 0)
			{
				PlayAnimationReverse(GrayAmberGauge_Enabled);
			} else
			{
				PlayAnimationForward(GrayAmberGauge_Enabled);
			}
			break;
		}
	}
}
