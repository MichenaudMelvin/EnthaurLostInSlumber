// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/SaveStatics.h"
#include "Saves/DefaultSave.h"

#pragma region SaveFunctions

UDefaultSave* USaveStatics::CreateSaveBP(const TSubclassOf<UDefaultSave> SaveClass, const int SaveIndex)
{
	UDefaultSave* NewSave = Cast<UDefaultSave>(UGameplayStatics::CreateSaveGameObject(SaveClass));
	NewSave->SetSaveIndex(SaveIndex);
	NewSave = SaveToSlot(NewSave, SaveIndex);

	return NewSave;
}

UDefaultSave* USaveStatics::SaveToSlot(UDefaultSave* SaveObject, const int SaveIndex)
{
	FString SlotName = SaveObject->GetSlotName();
	SlotName += FString::FromInt(SaveIndex);

	UGameplayStatics::AsyncSaveGameToSlot(SaveObject, SlotName, 0);

	return SaveObject;
}

UDefaultSave* USaveStatics::LoadSaveBP(const TSubclassOf<UDefaultSave> SaveClass, const int SaveIndex, const bool bCreateNewSaveIfDoesntExist)
{
	FString SlotName = SaveClass.GetDefaultObject()->GetSlotName();
	SlotName += FString::FromInt(SaveIndex);

	UDefaultSave* LoadedSave = Cast<UDefaultSave>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

	if (IsValid(LoadedSave))
	{
		LoadedSave->SetSaveIndex(SaveIndex);

		return LoadedSave;
	}

	if (bCreateNewSaveIfDoesntExist)
	{
		return CreateSaveBP(SaveClass, SaveIndex);
	}

	return nullptr;
}

bool USaveStatics::DeleteSaveSlot(UDefaultSave* SaveObject, const int SaveIndex)
{
	FString SlotName = SaveObject->GetSlotName();
	SlotName += FString::FromInt(SaveIndex);

	return UGameplayStatics::DeleteGameInSlot(SlotName, 0);
}

#pragma endregion