// Fill out your copyright notice in the Description page of Project Settings.


#include "ENTCustomCheatManager.h"
#include "ENTCustomDebugCamera.h"

UENTCustomCheatManager::UENTCustomCheatManager()
{
	DebugCameraControllerClass = AENTCustomDebugCamera::StaticClass();
}
