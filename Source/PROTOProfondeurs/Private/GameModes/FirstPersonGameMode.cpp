// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/FirstPersonGameMode.h"
#include "Subsystems/ENTWorldSaveSubsystem.h"

void AFirstPersonGameMode::SaveWorld() const
{
	UENTWorldSaveSubsystem* WorldSaveSubsystem = GetGameInstance()->GetSubsystem<UENTWorldSaveSubsystem>();
	if (!WorldSaveSubsystem)
	{
		return;
	}

	WorldSaveSubsystem->SaveToSlot(0);
}

void AFirstPersonGameMode::LoadWorld() const
{
	UENTWorldSaveSubsystem* WorldSaveSubsystem = GetGameInstance()->GetSubsystem<UENTWorldSaveSubsystem>();
	if (!WorldSaveSubsystem)
	{
		return;
	}

	WorldSaveSubsystem->LoadSave(0, false);
}
