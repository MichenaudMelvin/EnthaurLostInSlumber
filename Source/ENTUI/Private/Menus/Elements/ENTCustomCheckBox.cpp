// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Elements/ENTCustomCheckBox.h"

void UENTCustomCheckBox::SetIsOn(bool bInValue, bool bSkip)
{
	bIsOn = bInValue;
	OnCheckBoxStateChanged.Broadcast(bIsOn, bSkip);
}

void UENTCustomCheckBox::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (CustomCheckBoxButton)
	{
		CustomCheckBoxButton->OnClicked.AddDynamic(this, &UENTCustomCheckBox::HandleButtonClick);
	}
}

void UENTCustomCheckBox::BeginDestroy()
{
	Super::BeginDestroy();

	if (CustomCheckBoxButton)
	{
		CustomCheckBoxButton->OnClicked.RemoveDynamic(this, &UENTCustomCheckBox::HandleButtonClick);
	}
}

void UENTCustomCheckBox::HandleButtonClick()
{
	OnCheckBoxStateChanged.Broadcast(bIsOn, false);
}
