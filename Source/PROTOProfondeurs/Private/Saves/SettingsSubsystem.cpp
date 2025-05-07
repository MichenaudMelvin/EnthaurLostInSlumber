// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/SettingsSubsystem.h"

USettingsSubsystem::USettingsSubsystem()
{
	SaveClass = USettingsSave::StaticClass();
}

void USettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadSave(0, true);
	Settings = Cast<USettingsSave>(SaveObject);
}

void USettingsSubsystem::Deinitialize()
{
	Super::Deinitialize();
}
