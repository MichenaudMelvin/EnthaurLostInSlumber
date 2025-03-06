// Fill out your copyright notice in the Description page of Project Settings.


#include "Kevin/UI/InGameUI.h"

#include "FCTween.h"
#include "Components/Image.h"

void UInGameUI::NativeConstruct()
{
	Super::NativeConstruct();
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
	} else
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
