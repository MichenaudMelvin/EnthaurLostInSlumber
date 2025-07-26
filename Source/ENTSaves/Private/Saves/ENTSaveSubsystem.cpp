// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ENTSaveSubsystem.h"
#include "Saves/ENTDefaultSave.h"

#pragma region SaveFunctions

UENTDefaultSave* UENTSaveSubsystem::CreateSave(const int SaveIndex)
{
	if (!SaveClass)
	{
		return nullptr;
	}

	SaveObject = Cast<UENTDefaultSave>(UGameplayStatics::CreateSaveGameObject(SaveClass));
	SaveObject->SetSaveIndex(SaveIndex);
	SaveObject = SaveToSlot(SaveIndex);
	return SaveObject;
}

bool UENTSaveSubsystem::DeleteSave(const int SaveIndex)
{
	if (!SaveClass || !SaveObject)
	{
		return false;
	}

	FString SlotName = SaveObject->GetSlotName();
	SlotName += FString::FromInt(SaveIndex);

	return UGameplayStatics::DeleteGameInSlot(SlotName, 0);
}

UENTDefaultSave* UENTSaveSubsystem::SaveToSlot(const int SaveIndex)
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

UENTDefaultSave* UENTSaveSubsystem::LoadSave(const int SaveIndex, const bool bCreateNewSaveIfDoesntExist)
{
	if (!SaveClass)
	{
		return nullptr;
	}

	FString SlotName = SaveClass.GetDefaultObject()->GetSlotName();
	SlotName += FString::FromInt(SaveIndex);

	SaveObject = Cast<UENTDefaultSave>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

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

bool UENTSaveSubsystem::DoesSaveGameExist(const int SaveIndex)
{
	return IsValid(LoadSave(0, false));
}

void UENTSaveSubsystem::ResetSaveToDefault(const int SaveIndex)
{
	CreateSave(SaveIndex);
}

#pragma endregion
