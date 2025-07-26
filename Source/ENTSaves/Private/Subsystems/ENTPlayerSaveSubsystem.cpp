// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ENTPlayerSaveSubsystem.h"

#include "Config/ENTSavesConfig.h"
#include "GameFramework/GameModeBase.h"
#include "Saves/ENTPlayerSave.h"
#include "Subsystems/ENTWorldSaveSubsystem.h"

UENTPlayerSaveSubsystem::UENTPlayerSaveSubsystem()
{
	SaveClass = UENTPlayerSave::StaticClass();
}

UENTDefaultSave* UENTPlayerSaveSubsystem::CreateSave(const int SaveIndex)
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

	Super::CreateSave(SaveIndex);
	if (!SaveObject)
	{
		return nullptr;
	}

	UENTPlayerSave* CastSave = Cast<UENTPlayerSave>(SaveObject);
	if (!CastSave)
	{
		return nullptr;
	}

	PlayerSave = CastSave;
	return PlayerSave;
}

UENTDefaultSave* UENTPlayerSaveSubsystem::LoadSave(const int SaveIndex, const bool bCreateNewSaveIfDoesntExist)
{
	Super::LoadSave(SaveIndex, bCreateNewSaveIfDoesntExist);

	if (!SaveObject)
	{
		return nullptr;
	}

	UENTPlayerSave* CastSave = Cast<UENTPlayerSave>(SaveObject);
	if (!CastSave)
	{
		return nullptr;
	}

	PlayerSave = CastSave;
	return PlayerSave;
}

void UENTPlayerSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	WorldInitDelegateHandle = FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UENTPlayerSaveSubsystem::OnNewWorldStarted);
}

void UENTPlayerSaveSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnWorldInitializedActors.Remove(WorldInitDelegateHandle);
}

void UENTPlayerSaveSubsystem::OnNewWorldStarted(const FActorsInitializedParams& ActorsInitializedParams)
{
	const UENTSavesConfig* Config = GetDefault<UENTSavesConfig>();
	if (!Config)
	{
		return;
	}

	if (!Config->AllowedGameModes.Contains(GetWorld()->GetAuthGameMode()->GetClass()))
	{
		return;
	}

	if (!DoesSaveGameExist(0))
	{
		if (!CreateSave(0))
		{
			return;
		}
	}
	else
	{
		LoadSave(0, false);
	}

	if (!PlayerSave)
	{
		return;
	}

	PlayerSave->LastWorldSaved = GetWorld()->GetFName();
	SaveToSlot(0);
}

void UENTPlayerSaveSubsystem::StartNewGame()
{
	const UENTSavesConfig* Config = GetDefault<UENTSavesConfig>();
	if (!Config || Config->StartGameplayWorld.IsNull())
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Invalid StartGameplayWorld in UENTSavesConfig"));

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
		return;
	}

	UENTWorldSaveSubsystem* WorldSaveSubsystem = GetGameInstance()->GetSubsystem<UENTWorldSaveSubsystem>();
	if (WorldSaveSubsystem)
	{
		WorldSaveSubsystem->DeleteAllWorldSaves(0);
	}

	DeleteSave(0);
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, Config->StartGameplayWorld, true);
}

void UENTPlayerSaveSubsystem::ContinueGame()
{
	if (!LoadSave(0, false))
	{
		return;
	}

	UGameplayStatics::OpenLevel(this, PlayerSave->LastWorldSaved);
}
