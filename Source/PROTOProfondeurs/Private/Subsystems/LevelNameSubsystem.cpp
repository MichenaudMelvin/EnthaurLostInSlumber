// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/LevelNameSubsystem.h"

#include "Parameters/BPRefParameters.h"


void ULevelNameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	WorldInitDelegateHandle = FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &ULevelNameSubsystem::SetupWorldName);
}

void ULevelNameSubsystem::Deinitialize()
{
	Super::Deinitialize();
	FWorldDelegates::OnWorldInitializedActors.Remove(WorldInitDelegateHandle);
}

void ULevelNameSubsystem::SetupWorldName(const FActorsInitializedParams& ActorsInitializedParams)
{
	auto LevelNames = GetDefault<UBPRefParameters>()->LevelNames;
	auto PathName = ActorsInitializedParams.World->GetName();
	FDuoText* LevelName = LevelNames.Find(PathName);
	if (!LevelName)
	{
		return;
	}

	CurrentLevelName = *LevelName;
}
