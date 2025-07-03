// Fill out your copyright notice in the Description page of Project Settings.


#include "PRFCustomCheatManager.h"
#include "PRFCustomDebugCamera.h"

UPRFCustomCheatManager::UPRFCustomCheatManager()
{
	DebugCameraControllerClass = APRFCustomDebugCamera::StaticClass();
}
