// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EPRFUIState : uint8
{
	Gameplay,
	PauseMenu,
	MainMenu,
	AnyMenu
};