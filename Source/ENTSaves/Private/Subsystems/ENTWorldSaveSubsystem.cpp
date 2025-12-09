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
	WorldBeginTearDownDelegateHandle = FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UENTWorldSaveSubsystem::OnWorldBeginTearDown);
}

void UENTWorldSaveSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnWorldInitializedActors.Remove(WorldInitDelegateHandle);
	FWorldDelegates::OnWorldCleanup.Remove(WorldBeginTearDownDelegateHandle);
}

UENTDefaultSave* UENTWorldSaveSubsystem::CreateSave(const int SaveIndex)
{
	const UENTSavesConfig* Config = GetDefault<UENTSavesConfig>();
	if (!Config)
	{
		return nullptr;
	}

	if (!GetWorld())
	{
		return nullptr;
	}

	if (!GetWorld()->GetAuthGameMode())
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
	if (!bFinishLoading)
	{
		return nullptr;
	}

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

	CurrentWorldSave->SublevelsNames.Empty();
	for (ULevelStreaming* StreamingLevel : GetWorld()->GetStreamingLevels())
	{
		if (!StreamingLevel->IsLevelLoaded())
		{
			continue;
		}

		if (!StreamingLevel->GetWorldAsset())
		{
			continue;
		}

		CurrentWorldSave->SublevelsNames.Add(StreamingLevel->GetWorldAsset()->GetFName());
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

		IENTSaveGameElementInterface* SavedActor = Cast<IENTSaveGameElementInterface>(Actor);
		if (SavedActor)
		{
			SavedActor->SaveGameElement(CurrentWorldSave);
		}

		IENTSaveGameElementInterface::Execute_SaveGameElementBP(Actor, CurrentWorldSave);
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

	if (!GetWorld())
	{
		return nullptr;
	}

	if (!GetWorld()->GetAuthGameMode())
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

	bool bCannotLoadWorld = !CurrentWorldSave;

#if WITH_EDITOR
	const UENTEditorSettings* EditorSettings = GetDefault<UENTEditorSettings>();

	if (EditorSettings && !EditorSettings->bLoadLatestWorldSave)
	{
		bCannotLoadWorld = true;
	}
	else
	{
#endif
		if (GetWorld())
		{
			WorldBeginPlayDelegateHandle = GetWorld()->OnWorldBeginPlay.AddUObject(this, &UENTWorldSaveSubsystem::OnNewWorldBeginPlay);
		}
#if WITH_EDITOR
	}
#endif

	if (bCannotLoadWorld)
	{
		return;
	}

	bFinishLoading = false;

	LoadedLevelIndex = 0;
	bLoadedPlayer = false;
	UnloadSublevels();

	CurrentWorldSave->SublevelsNames.IsEmpty() ? FinishLoading() : LoadSublevels();
}

void UENTWorldSaveSubsystem::UnloadSublevels()
{
	for (ULevelStreaming* StreamingLevel : GetWorld()->GetStreamingLevels())
	{
		if (!StreamingLevel->GetWorldAsset())
		{
			continue;
		}

		bool bFindLevel = false;
		FName SublevelToUnload = StreamingLevel->GetWorldAsset()->GetFName();
		for (const FName& SublevelName : CurrentWorldSave->SublevelsNames)
		{
			if (SublevelName == SublevelToUnload)
			{
				bFindLevel = true;
				break;
			}
		}

		if (!bFindLevel)
		{
			UGameplayStatics::UnloadStreamLevel(this, SublevelToUnload, FLatentActionInfo(), false);
		}
	}
}

void UENTWorldSaveSubsystem::LoadSublevels()
{
	if (!CurrentWorldSave)
	{
		return;
	}

	FLatentActionInfo LatentActionInfo;
	LatentActionInfo.Linkage = 0;
	LatentActionInfo.UUID = LoadedLevelIndex;
	LatentActionInfo.CallbackTarget = this;
	LatentActionInfo.ExecutionFunction = CurrentWorldSave->SublevelsNames.Num() == LoadedLevelIndex + 1 ? "FinishLoading" : "LoadSublevels";

	UGameplayStatics::LoadStreamLevel(this, CurrentWorldSave->SublevelsNames[LoadedLevelIndex++], true, false, LatentActionInfo);
}

void UENTWorldSaveSubsystem::FinishLoading()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), Actors);

	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);

	// done before any BeginPlay()
	for (AActor* Actor : Actors)
	{
		if (!Actor->Implements<UENTSaveGameElementInterface>())
		{
			continue;
		}

		if (Player == Actor)
		{
			if (!bLoadedPlayer)
			{
				FENTGameElementData EmptyData;
				Cast<IENTSaveGameElementInterface>(Actor)->LoadGameElement(EmptyData, CurrentWorldSave);
				IENTSaveGameElementInterface::Execute_LoadGameElementBP(Actor, CurrentWorldSave);
				bLoadedPlayer = true;
			}

			continue;
		}

		IENTSaveGameElementInterface* InterfaceActor = Cast<IENTSaveGameElementInterface>(Actor);

		if (!InterfaceActor)
		{
			IENTSaveGameElementInterface::Execute_LoadGameElementBP(Actor, CurrentWorldSave);
			continue;
		}

		// TODO refactor
		FENTMuscleData* MuscleDataPtr = CurrentWorldSave->MuscleData.Find(Actor->GetName());
		if (MuscleDataPtr)
		{
			InterfaceActor->LoadGameElement(*MuscleDataPtr, CurrentWorldSave);
			IENTSaveGameElementInterface::Execute_LoadGameElementBP(Actor, CurrentWorldSave);
			continue;
		}

		FENTNerveData* NerveDataPtr = CurrentWorldSave->NerveData.Find(Actor->GetName());
		if (NerveDataPtr)
		{
			InterfaceActor->LoadGameElement(*NerveDataPtr, CurrentWorldSave);
			IENTSaveGameElementInterface::Execute_LoadGameElementBP(Actor, CurrentWorldSave);
			continue;
		}

		FENTAmberOreData* AmberOreDataPtr = CurrentWorldSave->AmberOreData.Find(Actor->GetName());
		if (AmberOreDataPtr)
		{
			InterfaceActor->LoadGameElement(*AmberOreDataPtr, CurrentWorldSave);
			IENTSaveGameElementInterface::Execute_LoadGameElementBP(Actor, CurrentWorldSave);
			continue;
		}

		FENTWeakZoneData* WeakZoneDataPtr = CurrentWorldSave->WeakZoneData.Find(Actor->GetName());
		if (WeakZoneDataPtr)
		{
			InterfaceActor->LoadGameElement(*WeakZoneDataPtr, CurrentWorldSave);
			IENTSaveGameElementInterface::Execute_LoadGameElementBP(Actor, CurrentWorldSave);
			continue;
		}

		FENTRespawnTreeData* RespawnTreeData = CurrentWorldSave->RespawnTreeData.Find(Actor->GetName());
		if (RespawnTreeData)
		{
			InterfaceActor->LoadGameElement(*RespawnTreeData, CurrentWorldSave);
			IENTSaveGameElementInterface::Execute_LoadGameElementBP(Actor, CurrentWorldSave);
			continue;
		}

		FENTParasiteData* ParaSiteData = CurrentWorldSave->ParasiteData.Find(Actor->GetName());
		if (ParaSiteData)
		{
			InterfaceActor->LoadGameElement(*ParaSiteData, CurrentWorldSave);
			IENTSaveGameElementInterface::Execute_LoadGameElementBP(Actor, CurrentWorldSave);
			continue;
		}

		FETNScriptedAIElementData* ScriptedAIElementData = CurrentWorldSave->ScriptedAIElementsData.Find(Actor->GetName());
		if (ScriptedAIElementData)
		{
			InterfaceActor->LoadGameElement(*ScriptedAIElementData, CurrentWorldSave);
			IENTSaveGameElementInterface::Execute_LoadGameElementBP(Actor, CurrentWorldSave);
			continue;
		}

		// if never found, delete the actor
		Actor->Destroy();
	}

	bFinishLoading = true;
}

void UENTWorldSaveSubsystem::OnNewWorldBeginPlay()
{
	GetWorld()->OnWorldBeginPlay.Remove(WorldBeginPlayDelegateHandle);

	if (bLoadedPlayer)
	{
		return;
	}

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
	Cast<IENTSaveGameElementInterface>(Character)->LoadGameElement(EmptyData, CurrentWorldSave);
	IENTSaveGameElementInterface::Execute_LoadGameElementBP(Character, CurrentWorldSave);

	bLoadedPlayer = true;
}

void UENTWorldSaveSubsystem::OnWorldBeginTearDown(UWorld* World)
{
	CurrentWorldSave = nullptr;
	bLoadedPlayer = false;
}
