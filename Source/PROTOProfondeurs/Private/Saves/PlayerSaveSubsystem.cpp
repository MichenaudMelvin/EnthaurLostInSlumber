// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/PlayerSaveSubsystem.h"
#include "Saves/PlayerSave.h"

UPlayerSaveSubsystem::UPlayerSaveSubsystem()
{
	SaveClass = UPlayerSave::StaticClass();
}

void UPlayerSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadSave(0, true);
	PlayerSave = Cast<UPlayerSave>(SaveObject);
}
