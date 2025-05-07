// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/WorldSaves/WorldSaveSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Saves/WorldSaves/WorldSave.h"
#include "Saves/WorldSaves/WorldSaveSettings.h"

UWorldSaveSubsystem::UWorldSaveSubsystem()
{
	SaveClass = UWorldSave::StaticClass();
}

void UWorldSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	WorldBeginPlayDelegateHandle = FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UWorldSaveSubsystem::OnNewWorldStarted);
}

void UWorldSaveSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnWorldInitializedActors.Remove(WorldBeginPlayDelegateHandle);
}

void UWorldSaveSubsystem::CreateSave(const int SaveIndex)
{
	const UWorldSaveSettings* Settings = GetDefault<UWorldSaveSettings>();
	if (!Settings)
	{
		return;
	}

	if (!Settings->AllowedGameModes.Contains(GetWorld()->GetAuthGameMode()->GetClass()))
	{
		return;
	}

	if (!SaveClass)
	{
		return;
	}

	SaveObject = Cast<UDefaultSave>(UGameplayStatics::CreateSaveGameObject(SaveClass));
	SaveObject->SetSaveIndex(SaveIndex);

	CurrentWorldSave = Cast<UWorldSave>(SaveObject);
	CurrentWorldSave->WorldName = GetWorld()->GetName();

	SaveObject = SaveToSlot(SaveIndex);
}

UDefaultSave* UWorldSaveSubsystem::LoadSave(const int SaveIndex, const bool bCreateNewSaveIfDoesntExist)
{
	const UWorldSaveSettings* Settings = GetDefault<UWorldSaveSettings>();
	if (!Settings)
	{
		return nullptr;
	}

	if (!Settings->AllowedGameModes.Contains(GetWorld()->GetAuthGameMode()->GetClass()))
	{
		return nullptr;
	}

	if (!SaveClass)
	{
		return nullptr;
	}

	FString SlotName = GetWorld()->GetName() + SaveClass.GetDefaultObject()->GetSlotName();
	SlotName += FString::FromInt(SaveIndex);

	SaveObject = Cast<UDefaultSave>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	CurrentWorldSave = Cast<UWorldSave>(SaveObject);

	if (IsValid(SaveObject))
	{
		SaveObject->SetSaveIndex(SaveIndex);

		return SaveObject;
	}

	if (bCreateNewSaveIfDoesntExist)
	{
		CreateSave(SaveIndex);
		return SaveObject;
	}

	return nullptr;
}

void UWorldSaveSubsystem::OnNewWorldStarted(const FActorsInitializedParams& ActorsInitializedParams)
{
	LoadSave(0, true);
}
