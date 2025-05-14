// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/FirstPersonGameMode.h"
#include "Saves/WorldSaves/WorldSaveSubsystem.h"

void AFirstPersonGameMode::SaveWorld() const
{
	UWorldSaveSubsystem* WorldSaveSubsystem = GetGameInstance()->GetSubsystem<UWorldSaveSubsystem>();
	if (!WorldSaveSubsystem)
	{
		return;
	}

	WorldSaveSubsystem->SaveToSlot(0);
}

void AFirstPersonGameMode::LoadWorld() const
{
	UWorldSaveSubsystem* WorldSaveSubsystem = GetGameInstance()->GetSubsystem<UWorldSaveSubsystem>();
	if (!WorldSaveSubsystem)
	{
		return;
	}

	WorldSaveSubsystem->LoadSave(0, false);
}
