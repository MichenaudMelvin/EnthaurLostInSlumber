// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Elements/ENTInputSlot.h"

#include "Components/Button.h"
#include "Menus/Elements/ENTCustomButton.h"
#include "Menus/Options/ENTControlsMenu.h"

void UENTInputSlot::SetKeyMappingName(const FName& InKeyMappingName)
{
	KeyMappingName = InKeyMappingName;
}

void UENTInputSlot::SetButtonKeyName(const FText& InButtonKeyName)
{
	ButtonKeyNameText = InButtonKeyName;
	Button->GetButtonText()->SetText(ButtonKeyNameText);
}

void UENTInputSlot::SetControlsMenu(UENTControlsMenu* InControlsMenu)
{
	ControlsMenu = InControlsMenu;
}

FName UENTInputSlot::GetMappingName()
{
	return KeyMappingName;
}

void UENTInputSlot::NativeConstruct()
{
	Super::NativeConstruct();

	Button->GetButtonText()->SetText(ButtonKeyNameText);
}

void UENTInputSlot::SetKeyName(const FText& InKeyName)
{
	KeyName->SetText(InKeyName);
}

void UENTInputSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button && Button->GetCustomButton())
	{
		Button->GetCustomButton()->OnPressed.AddDynamic(this, &UENTInputSlot::OnKeyButtonPressed);
	}
}

void UENTInputSlot::BeginDestroy()
{
	Super::BeginDestroy();

	if (Button && Button->GetCustomButton())
	{
		Button->GetCustomButton()->OnPressed.RemoveDynamic(this, &UENTInputSlot::OnKeyButtonPressed);
	}
}

void UENTInputSlot::OnKeyButtonPressed()
{
	ControlsMenu->OnKeyButton(this);
	SetKeyboardFocus();
}
