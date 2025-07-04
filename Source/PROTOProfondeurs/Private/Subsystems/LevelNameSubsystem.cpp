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
	FString SourcePath;
	FString RemappedPath;
	ActorsInitializedParams.World->GetSoftObjectPathMapping(SourcePath, RemappedPath);

	FSoftObjectPath SoftObjectPath(SourcePath);
	TSoftObjectPtr<UWorld> CurrentWorldSoftPtr(SoftObjectPath);

	TMap<TSoftObjectPtr<UWorld>, FDuoText> LevelNames = GetDefault<UBPRefParameters>()->LevelNames;

	FDuoText* LevelName = LevelNames.Find(CurrentWorldSoftPtr);
	if (!LevelName)
	{
		return;
	}

	CurrentLevelName = *LevelName;
}
