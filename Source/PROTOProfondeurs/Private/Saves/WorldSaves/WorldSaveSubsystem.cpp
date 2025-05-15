// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/WorldSaves/WorldSaveSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Player/FirstPersonCharacter.h"
#include "Saves/PlayerSave.h"
#include "Saves/PlayerSaveSubsystem.h"
#include "Saves/WorldSaves/WorldSave.h"
#include "Saves/WorldSaves/WorldSaveSettings.h"

#if WITH_EDITOR
#include "EditorSettings/SavesSettings.h"
#endif

UWorldSaveSubsystem::UWorldSaveSubsystem()
{
	SaveClass = UWorldSave::StaticClass();
}

void UWorldSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	WorldInitDelegateHandle = FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UWorldSaveSubsystem::OnNewWorldStarted);
}

void UWorldSaveSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnWorldInitializedActors.Remove(WorldInitDelegateHandle);
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

UDefaultSave* UWorldSaveSubsystem::SaveToSlot(const int SaveIndex)
{
	if (!CurrentWorldSave)
	{
		CreateSave(SaveIndex);
	}

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), Actors);

	CurrentWorldSave->ClearMapData();

	for (AActor* Actor : Actors)
	{
		if (!Actor->Implements<USaveGameElementInterface>())
		{
			continue;
		}

		Cast<ISaveGameElementInterface>(Actor)->SaveGameElement(CurrentWorldSave);
	}

	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Character)
	{
		return Super::SaveToSlot(SaveIndex);
	}

	AFirstPersonCharacter* FirstPersonCharacter = Cast<AFirstPersonCharacter>(Character);
	if (!FirstPersonCharacter && !FirstPersonCharacter->GetRespawnTree())
	{
		return Super::SaveToSlot(SaveIndex);
	}

	FirstPersonCharacter->SavePlayerData();
	CurrentWorldSave->LastCheckPointName = FirstPersonCharacter->GetRespawnTree() ? FirstPersonCharacter->GetRespawnTree().GetName() : "";

	return Super::SaveToSlot(SaveIndex);
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
	LoadSave(0, false);

	bool bCannotLoad = !CurrentWorldSave;

#if WITH_EDITOR
	const USavesSettings* SavesSettings = GetDefault<USavesSettings>();

	if (SavesSettings && !SavesSettings->bLoadLatestWorldSave)
	{
		bCannotLoad = true;
	}
#endif

	if (bCannotLoad)
	{
		return;
	}

	if (GetWorld())
	{
		WorldBeginPlayDelegateHandle = GetWorld()->OnWorldBeginPlay.AddUObject(this, &UWorldSaveSubsystem::OnNewWorldBeginPlay);
	}

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), Actors);

	// done before any BeginPlay()
	for (AActor* Actor : Actors)
	{
		if (!Actor->Implements<USaveGameElementInterface>())
		{
			continue;
		}

		// TODO refactor
		FMuscleData* MuscleDataPtr = CurrentWorldSave->MuscleData.Find(Actor->GetName());
		if (MuscleDataPtr)
		{
			Cast<ISaveGameElementInterface>(Actor)->LoadGameElement(*MuscleDataPtr);
			continue;
		}

		FNerveData* NerveDataPtr = CurrentWorldSave->NerveData.Find(Actor->GetName());
		if (NerveDataPtr)
		{
			Cast<ISaveGameElementInterface>(Actor)->LoadGameElement(*NerveDataPtr);
			continue;
		}

		FAmberOreData* AmberOreDataPtr = CurrentWorldSave->AmberOreData.Find(Actor->GetName());
		if (AmberOreDataPtr)
		{
			Cast<ISaveGameElementInterface>(Actor)->LoadGameElement(*AmberOreDataPtr);
			continue;
		}

		FWeakZoneData* WeakZoneDataPtr = CurrentWorldSave->WeakZoneData.Find(Actor->GetName());
		if (WeakZoneDataPtr)
		{
			Cast<ISaveGameElementInterface>(Actor)->LoadGameElement(*WeakZoneDataPtr);
			continue;
		}

		FRespawnTreeData* RespawnTreeData = CurrentWorldSave->RespawnTreeData.Find(Actor->GetName());
		if (RespawnTreeData)
		{
			Cast<ISaveGameElementInterface>(Actor)->LoadGameElement(*RespawnTreeData);
			continue;
		}

		FParaSiteData* ParaSiteData = CurrentWorldSave->ParasiteData.Find(Actor->GetName());
		if (ParaSiteData)
		{
			Cast<ISaveGameElementInterface>(Actor)->LoadGameElement(*ParaSiteData);
			continue;
		}

		// if nerved found, delete the actor
		Actor->Destroy();
	}
}

void UWorldSaveSubsystem::OnNewWorldBeginPlay()
{
	GetWorld()->OnWorldBeginPlay.Remove(WorldBeginPlayDelegateHandle);

	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Character)
	{
		return;
	}

	AFirstPersonCharacter* FirstPersonCharacter = Cast<AFirstPersonCharacter>(Character);
	if (!FirstPersonCharacter)
	{
		return;
	}

	FirstPersonCharacter->LoadPlayerData();
}