// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/PRFCreditsMenu.h"

#include "Animation/WidgetAnimation.h"

void UPRFCreditsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (NamesAnimation)
	{
		PlayAnimation(NamesAnimation);
	}
}

void UPRFCreditsMenu::NativeDestruct()
{
	Super::NativeDestruct();
}
