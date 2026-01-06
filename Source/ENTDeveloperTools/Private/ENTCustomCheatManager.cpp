// Fill out your copyright notice in the Description page of Project Settings.


#include "ENTCustomCheatManager.h"
#include "ENTCustomDebugCamera.h"
#include "Blueprint/WidgetLayoutLibrary.h"

UENTCustomCheatManager::UENTCustomCheatManager()
{
	DebugCameraControllerClass = AENTCustomDebugCamera::StaticClass();
}

void UENTCustomCheatManager::RemoveAllWidgets()
{
	UWidgetLayoutLibrary::RemoveAllWidgets(this);
}
