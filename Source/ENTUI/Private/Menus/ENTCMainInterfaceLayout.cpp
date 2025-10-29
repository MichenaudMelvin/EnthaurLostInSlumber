// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/ENTCMainInterfaceLayout.h"

void UENTCMainInterfaceLayout::AddWidgetToStack(UCommonActivatableWidget* InWidget) const
{
	WidgetStack->AddWidgetInstance(*InWidget);
}

void UENTCMainInterfaceLayout::RemoveWidgetFromStack(UCommonActivatableWidget* InWidget) const
{
	WidgetStack->RemoveWidget(*InWidget);
}
