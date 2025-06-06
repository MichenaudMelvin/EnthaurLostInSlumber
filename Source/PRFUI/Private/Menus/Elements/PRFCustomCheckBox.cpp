// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Elements/PRFCustomCheckBox.h"

void UPRFCustomCheckBox::SetIsOn(bool bInValue, bool bSkip)
{
	bIsOn = bInValue;
	OnCheckBoxStateChanged.Broadcast(bIsOn, bSkip);
}

void UPRFCustomCheckBox::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (CustomCheckBoxButton)
	{
		CustomCheckBoxButton->OnClicked.AddDynamic(this, &UPRFCustomCheckBox::HandleButtonClick);
	}
}

void UPRFCustomCheckBox::BeginDestroy()
{
	Super::BeginDestroy();

	if (CustomCheckBoxButton)
	{
		CustomCheckBoxButton->OnClicked.RemoveDynamic(this, &UPRFCustomCheckBox::HandleButtonClick);
	}
}

void UPRFCustomCheckBox::HandleButtonClick()
{
	OnCheckBoxStateChanged.Broadcast(bIsOn, false);
}
