// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ENTLevelNameSubsystem.h"

void UENTLevelNameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	WorldInitDelegateHandle = FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UENTLevelNameSubsystem::SetupWorldName);
}

void UENTLevelNameSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnWorldInitializedActors.Remove(WorldInitDelegateHandle);
}

void UENTLevelNameSubsystem::SetupWorldName(const FActorsInitializedParams& ActorsInitializedParams)
{
	FString SourcePath;
	FString RemappedPath;
	ActorsInitializedParams.World->GetSoftObjectPathMapping(SourcePath, RemappedPath);

	FSoftObjectPath SoftObjectPath(SourcePath);
	TSoftObjectPtr<UWorld> CurrentWorldSoftPtr(SoftObjectPath);

	TMap<TSoftObjectPtr<UWorld>, FENTZoneName> ZoneNames = GetDefault<UENTUIConfig>()->ZoneNames;

	FENTZoneName* ZoneName = ZoneNames.Find(CurrentWorldSoftPtr);
	if (!ZoneName)
	{
		return;
	}

	CurrentZoneName = *ZoneName;
}
