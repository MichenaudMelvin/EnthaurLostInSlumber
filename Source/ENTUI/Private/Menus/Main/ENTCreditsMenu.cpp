// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/ENTCreditsMenu.h"

#include "Animation/WidgetAnimation.h"

void UENTCreditsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (NamesAnimation)
	{
		PlayAnimation(NamesAnimation);
	}
}

void UENTCreditsMenu::NativeDestruct()
{
	Super::NativeDestruct();
}
