// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Elements/ENTInputSlot.h"

#include "Components/Button.h"


void UENTInputSlot::SetKeyMappingName(const FName& InKeyMappingName)
{
	KeyMappingName = InKeyMappingName;
}

void UENTInputSlot::SetKeyName(const FText& InKeyName)
{
	KeyName->SetText(InKeyName);
}

void UENTInputSlot::SetButtonTextBlock(const FText& InButtonTextBlock)
{
	ButtonTextBlock->SetText(InButtonTextBlock);
}

void UENTInputSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button)
	{
		//Button->OnClicked.AddDynamic(this, );
	}
}

void UENTInputSlot::BeginDestroy()
{
	Super::BeginDestroy();

	if (Button)
	{
		//Button->OnClicked.RemoveDynamic(this, );
	}
}
