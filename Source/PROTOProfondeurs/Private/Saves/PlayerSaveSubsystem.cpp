// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/PlayerSaveSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Saves/PlayerSave.h"
#include "Saves/WorldSaves/WorldSaveSettings.h"
#include "Saves/WorldSaves/WorldSaveSubsystem.h"

UPlayerSaveSubsystem::UPlayerSaveSubsystem()
{
	SaveClass = UPlayerSave::StaticClass();
}

UDefaultSave* UPlayerSaveSubsystem::CreateSave(const int SaveIndex)
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

	Super::CreateSave(SaveIndex);
	if (!SaveObject)
	{
		return nullptr;
	}

	UPlayerSave* CastSave = Cast<UPlayerSave>(SaveObject);
	if (!CastSave)
	{
		return nullptr;
	}

	PlayerSave = CastSave;
	return PlayerSave;
}

UDefaultSave* UPlayerSaveSubsystem::LoadSave(const int SaveIndex, const bool bCreateNewSaveIfDoesntExist)
{
	Super::LoadSave(SaveIndex, bCreateNewSaveIfDoesntExist);

	if (!SaveObject)
	{
		return nullptr;
	}

	UPlayerSave* CastSave = Cast<UPlayerSave>(SaveObject);
	if (!CastSave)
	{
		return nullptr;
	}

	PlayerSave = CastSave;
	return PlayerSave;
}

void UPlayerSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	WorldInitDelegateHandle = FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UPlayerSaveSubsystem::OnNewWorldStarted);
}

void UPlayerSaveSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnWorldInitializedActors.Remove(WorldInitDelegateHandle);
}

void UPlayerSaveSubsystem::OnNewWorldStarted(const FActorsInitializedParams& ActorsInitializedParams)
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

void UPlayerSaveSubsystem::StartNewGame()
{
	const UWorldSaveSettings* WorldSaveSettings = GetDefault<UWorldSaveSettings>();
	if (!WorldSaveSettings || WorldSaveSettings->StartGameplayWorld.IsNull())
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Invalid StartGameplayWorld in WorldSaveSettings"));

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
		return;
	}

	UWorldSaveSubsystem* WorldSaveSubsystem = GetGameInstance()->GetSubsystem<UWorldSaveSubsystem>();
	if (WorldSaveSubsystem)
	{
		WorldSaveSubsystem->DeleteAllWorldSaves(0);
	}

	DeleteSave(0);
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, WorldSaveSettings->StartGameplayWorld, true);
}

void UPlayerSaveSubsystem::ContinueGame()
{
	if (!LoadSave(0, false))
	{
		return;
	}

	UGameplayStatics::OpenLevel(this, PlayerSave->LastWorldSaved);
}
