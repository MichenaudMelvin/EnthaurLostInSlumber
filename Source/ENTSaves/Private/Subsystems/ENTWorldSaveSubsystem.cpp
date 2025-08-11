// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ENTWorldSaveSubsystem.h"
#include "Config/ENTSavesConfig.h"
#include "GameFramework/Character.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "GameFramework/GameModeBase.h"
#include "Saves/WorldSaves/ENTSaveGameElementInterface.h"
#include "Subsystems/ENTPlayerSaveSubsystem.h"
#include "Saves/WorldSaves/ENTWorldSave.h"

#if WITH_EDITOR
#include "EditorSettings/ENTEditorSettings.h"
#endif

UENTWorldSaveSubsystem::UENTWorldSaveSubsystem()
{
	SaveClass = UENTWorldSave::StaticClass();
}

void UENTWorldSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	WorldInitDelegateHandle = FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UENTWorldSaveSubsystem::OnNewWorldStarted);
}

void UENTWorldSaveSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnWorldInitializedActors.Remove(WorldInitDelegateHandle);
}

UENTDefaultSave* UENTWorldSaveSubsystem::CreateSave(const int SaveIndex)
{
	const UENTSavesConfig* Config = GetDefault<UENTSavesConfig>();
	if (!Config)
	{
		return nullptr;
	}

	if (!Config->AllowedGameModes.Contains(GetWorld()->GetAuthGameMode()->GetClass()))
	{
		return nullptr;
	}

	if (!SaveClass)
	{
		return nullptr;
	}

	SaveObject = Cast<UENTDefaultSave>(UGameplayStatics::CreateSaveGameObject(SaveClass));
	SaveObject->SetSaveIndex(SaveIndex);

	CurrentWorldSave = Cast<UENTWorldSave>(SaveObject);
	CurrentWorldSave->WorldName = GetWorld()->GetName();

	SaveObject = SaveToSlot(SaveIndex);

	return CurrentWorldSave;
}

UENTDefaultSave* UENTWorldSaveSubsystem::SaveToSlot(const int SaveIndex)
{
	if (!CurrentWorldSave)
	{
		CreateSave(SaveIndex);

		if (!CurrentWorldSave)
		{
#if WITH_EDITOR
			const FString Message = FString::Printf(TEXT("Cannot create save, check allowed GameModes"));

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
			FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
			return nullptr;
		}
	}

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), Actors);

	CurrentWorldSave->ClearMapData();

	for (AActor* Actor : Actors)
	{
		if (!Actor->Implements<UENTSaveGameElementInterface>())
		{
			continue;
		}

		Cast<IENTSaveGameElementInterface>(Actor)->SaveGameElement(CurrentWorldSave);
	}

	return Super::SaveToSlot(SaveIndex);
}

UENTDefaultSave* UENTWorldSaveSubsystem::LoadSave(const int SaveIndex, const bool bCreateNewSaveIfDoesntExist)
{
	const UENTSavesConfig* Config = GetDefault<UENTSavesConfig>();
	if (!Config)
	{
		return nullptr;
	}

	if (!Config->AllowedGameModes.Contains(GetWorld()->GetAuthGameMode()->GetClass()))
	{
		return nullptr;
	}

	if (!SaveClass)
	{
		return nullptr;
	}

	FString SlotName = GetWorld()->GetName() + SaveClass.GetDefaultObject()->GetSlotName();
	SlotName += FString::FromInt(SaveIndex);

	SaveObject = Cast<UENTDefaultSave>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	CurrentWorldSave = Cast<UENTWorldSave>(SaveObject);

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

bool UENTWorldSaveSubsystem::DeleteWorldSaveByName(const FString& WorldName, const int SaveIndex)
{
	if (!SaveClass)
	{
		return false;
	}

	FString WorldSaveName = WorldName + SaveClass.GetDefaultObject()->GetSlotName() + FString::FromInt(SaveIndex);
	return UGameplayStatics::DeleteGameInSlot(WorldSaveName, 0);
}

void UENTWorldSaveSubsystem::DeleteAllWorldSaves(const int SaveIndex)
{
	const UENTSavesConfig* Config = GetDefault<UENTSavesConfig>();
	if (!Config)
	{
		return;
	}

	TArray<FPrimaryAssetId> PrimaryMapAssetIds;
	UKismetSystemLibrary::GetPrimaryAssetIdList(Config->PrimaryMapAsset, PrimaryMapAssetIds);

	for (const FPrimaryAssetId& PrimaryMapAssetId : PrimaryMapAssetIds)
	{
		FString PathSeparation = "/";
		FString PathName;
		FString LevelName;
		PrimaryMapAssetId.PrimaryAssetName.ToString().Split(PathSeparation, &PathName, &LevelName, ESearchCase::CaseSensitive, ESearchDir::FromEnd);

		DeleteWorldSaveByName(LevelName, SaveIndex);
	}
}

void UENTWorldSaveSubsystem::OnNewWorldStarted(const FActorsInitializedParams& ActorsInitializedParams)
{
	LoadSave(0, false);

	bool bCannotLoad = !CurrentWorldSave;

#if WITH_EDITOR
	const UENTEditorSettings* EditorSettings = GetDefault<UENTEditorSettings>();

	if (EditorSettings && !EditorSettings->bLoadLatestWorldSave)
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
		WorldBeginPlayDelegateHandle = GetWorld()->OnWorldBeginPlay.AddUObject(this, &UENTWorldSaveSubsystem::OnNewWorldBeginPlay);
	}

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), Actors);

	// done before any BeginPlay()
	for (AActor* Actor : Actors)
	{
		if (!Actor->Implements<UENTSaveGameElementInterface>())
		{
			continue;
		}

		// TODO refactor
		FENTMuscleData* MuscleDataPtr = CurrentWorldSave->MuscleData.Find(Actor->GetName());
		if (MuscleDataPtr)
		{
			Cast<IENTSaveGameElementInterface>(Actor)->LoadGameElement(*MuscleDataPtr);
			continue;
		}

		FENTNerveData* NerveDataPtr = CurrentWorldSave->NerveData.Find(Actor->GetName());
		if (NerveDataPtr)
		{
			Cast<IENTSaveGameElementInterface>(Actor)->LoadGameElement(*NerveDataPtr);
			continue;
		}

		FENTAmberOreData* AmberOreDataPtr = CurrentWorldSave->AmberOreData.Find(Actor->GetName());
		if (AmberOreDataPtr)
		{
			Cast<IENTSaveGameElementInterface>(Actor)->LoadGameElement(*AmberOreDataPtr);
			continue;
		}

		FENTWeakZoneData* WeakZoneDataPtr = CurrentWorldSave->WeakZoneData.Find(Actor->GetName());
		if (WeakZoneDataPtr)
		{
			Cast<IENTSaveGameElementInterface>(Actor)->LoadGameElement(*WeakZoneDataPtr);
			continue;
		}

		FENTRespawnTreeData* RespawnTreeData = CurrentWorldSave->RespawnTreeData.Find(Actor->GetName());
		if (RespawnTreeData)
		{
			Cast<IENTSaveGameElementInterface>(Actor)->LoadGameElement(*RespawnTreeData);
			continue;
		}

		FENTParaSiteData* ParaSiteData = CurrentWorldSave->ParasiteData.Find(Actor->GetName());
		if (ParaSiteData)
		{
			Cast<IENTSaveGameElementInterface>(Actor)->LoadGameElement(*ParaSiteData);
			continue;
		}

		// if never found, delete the actor
		Actor->Destroy();
	}
}

void UENTWorldSaveSubsystem::OnNewWorldBeginPlay()
{
	GetWorld()->OnWorldBeginPlay.Remove(WorldBeginPlayDelegateHandle);

	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Character)
	{
		return;
	}

	if (!Character->Implements<UENTSaveGameElementInterface>())
	{
		return;
	}

	FENTGameElementData EmptyData;
	Cast<IENTSaveGameElementInterface>(Character)->LoadGameElement(EmptyData);
}