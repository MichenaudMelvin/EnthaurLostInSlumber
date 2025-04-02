// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/SaveSubsystem.h"
#include "Saves/DefaultSave.h"

#pragma region SaveFunctions

void USaveSubsystem::CreateSave(const int SaveIndex)
{
	if (!SaveClass)
	{
		return;
	}

	SaveObject = Cast<UDefaultSave>(UGameplayStatics::CreateSaveGameObject(SaveClass));
	SaveObject->SetSaveIndex(SaveIndex);
	SaveObject = SaveToSlot(SaveIndex);
}

UDefaultSave* USaveSubsystem::SaveToSlot(const int SaveIndex)
{
	if (!SaveObject)
	{
		return nullptr;
	}

	FString SlotName = SaveObject->GetSlotName();
	SlotName += FString::FromInt(SaveIndex);

	UGameplayStatics::AsyncSaveGameToSlot(SaveObject, SlotName, 0);

	return SaveObject;
}

UDefaultSave* USaveSubsystem::LoadSave(const int SaveIndex, const bool bCreateNewSaveIfDoesntExist)
{
	if (!SaveClass)
	{
		return nullptr;
	}

	FString SlotName = SaveClass.GetDefaultObject()->GetSlotName();
	SlotName += FString::FromInt(SaveIndex);

	SaveObject = Cast<UDefaultSave>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

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

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadSave(0, true);
}

void USaveSubsystem::ResetSaveToDefault(const int SaveIndex)
{
	if (!SaveObject)
	{
		return;
	}

	CreateSave(SaveIndex);
}

#pragma endregion
