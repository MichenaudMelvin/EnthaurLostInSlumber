// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/ENTDefaultGameMode.h"
#include "Subsystems/ENTWorldSaveSubsystem.h"

void AENTDefaultGameMode::SaveWorld() const
{
	UENTWorldSaveSubsystem* WorldSaveSubsystem = GetGameInstance()->GetSubsystem<UENTWorldSaveSubsystem>();
	if (!WorldSaveSubsystem)
	{
		return;
	}

	WorldSaveSubsystem->SaveToSlot(0);
}

void AENTDefaultGameMode::LoadWorld() const
{
	UENTWorldSaveSubsystem* WorldSaveSubsystem = GetGameInstance()->GetSubsystem<UENTWorldSaveSubsystem>();
	if (!WorldSaveSubsystem)
	{
		return;
	}

	WorldSaveSubsystem->LoadSave(0, false);
}
