// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SaveStatics.generated.h"

class UDefaultSave;

UCLASS()
class PROTOPROFONDEURS_API USaveStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

#pragma region SaveFunctions

public:
	/**
	 * @brief Create a new save
	 * @param SaveClass The type of save you want to create
	 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
	 * @return The created save
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DeterminesOutputType = "SaveClass"), Category = "Saves", meta = (DisplayName = "CreateSave"))
	static UDefaultSave* CreateSaveBP(const TSubclassOf<UDefaultSave> SaveClass, const int SaveIndex);

	/**
	 * @brief Save to a slot
	 * @param SaveObject The affected save object
	 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
	 * @return The save object
	 */
	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "SaveObject"), Category = "Saves")
	static UDefaultSave* SaveToSlot(UDefaultSave* SaveObject, const int SaveIndex);

	/**
	 * @brief Load a save
	 * @param SaveClass The type of save you want to load
	 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
	 * @param bCreateNewSaveIfDoesntExist This will create a new save if the one you're trying to load doesn't exist
	 * @return The save object
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DeterminesOutputType = "SaveClass"), Category = "Saves", meta = (DisplayName = "LoadSave"))
	static UDefaultSave* LoadSaveBP(const TSubclassOf<UDefaultSave> SaveClass, const int SaveIndex, const bool bCreateNewSaveIfDoesntExist = true);

	/**
	 * @brief Delete a save
	 * @param SaveObject The save object
	 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
	 * @return Successfully delete the save or not
	 */
	UFUNCTION(BlueprintCallable, Category = "Saves")
	static bool DeleteSaveSlot(UDefaultSave* SaveObject, const int SaveIndex);

#pragma endregion
};

#pragma region TemplateFunctions

/**
 * @brief Create a new save
 * @tparam SaveT The type of save you want to create
 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
 * @return The created save
 */
template<typename SaveT = UDefaultSave>
inline SaveT* CreateSave(const int SaveIndex)
{
	SaveT* NewSave = Cast<SaveT>(UGameplayStatics::CreateSaveGameObject(SaveT::StaticClass()));
	NewSave->SetSaveIndex(SaveIndex);
	NewSave = Cast<SaveT>(USaveStatics::SaveToSlot(NewSave, SaveIndex));

	return NewSave;
}

/**
 * @brief Save to a slot
 * @tparam SaveT The type of save you want to load
 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
 * @param bCreateNewSaveIfDoesntExist his will create a new save if the one you're trying to load doesn't exist
 * @return The save object
 */
template<typename SaveT = UDefaultSave>
inline SaveT* LoadSave(const int SaveIndex, const bool bCreateNewSaveIfDoesntExist = true)
{
	TSubclassOf<SaveT> SaveClass = SaveT::StaticClass();
	FString SlotName = SaveClass.GetDefaultObject()->GetSlotName();
	SlotName += FString::FromInt(SaveIndex);

	SaveT* LoadedSave = Cast<SaveT>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

	if (IsValid(LoadedSave))
	{
		LoadedSave->SetSaveIndex(SaveIndex);
		return LoadedSave;
	}

	if (bCreateNewSaveIfDoesntExist)
	{
		return CreateSave<SaveT>(SaveIndex);
	}

	return nullptr;
}

#pragma endregion