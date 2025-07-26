// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Saves/ENTDefaultSave.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ENTSaveSubsystem.generated.h"

class UENTDefaultSave;

/**
 * @brief This subsystem is abstract
 */
UCLASS(Abstract, Category = "Save")
class ENTSAVES_API UENTSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

#pragma region SaveFunctions

public:
	/**
	 * @brief Create a new save
	 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
	 */
	UFUNCTION(BlueprintCallable, Category = "Saves")
	virtual UENTDefaultSave* CreateSave(const int SaveIndex);

	/**
	 * @brief Delete a save
	 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
	 * @return True if a file was actually able to be deleted. use DoesSaveGameExist to distinguish between delete failures and failure due to file not existing.
	 */
	UFUNCTION(BlueprintCallable, Category = "Saves")
	virtual bool DeleteSave(const int SaveIndex);

	/**
	 * @brief Save to a slot
	 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
	 * @return The save object
	 */
	UFUNCTION(BlueprintCallable, Category = "Saves")
	virtual UENTDefaultSave* SaveToSlot(const int SaveIndex);

	/**
	 * @brief Load a save
	 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
	 * @param bCreateNewSaveIfDoesntExist This will create a new save if the one you're trying to load doesn't exist
	 * @return The save object
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Saves")
	virtual UENTDefaultSave* LoadSave(const int SaveIndex, const bool bCreateNewSaveIfDoesntExist = true);

	/**
	 * @brief 
	 * @param SaveIndex 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Saves")
	bool DoesSaveGameExist(const int SaveIndex);

	/**
	 * @brief Reset a save to defaults parameters
	 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
	 */
	UFUNCTION(BlueprintCallable, Category = "Saves")
	void ResetSaveToDefault(const int SaveIndex);

#pragma endregion

protected:
	/**
	 * @brief Must be assigned in the subsystem ctor
	 */
	TSubclassOf<UENTDefaultSave> SaveClass;

	UPROPERTY()
	TObjectPtr<UENTDefaultSave> SaveObject;
};

#pragma region TemplateFunctions

/**
 * @brief Create a new save
 * @tparam SaveT The type of save you want to create
 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
 * @return The created save
 */
template<typename SaveT = UENTDefaultSave>
inline SaveT* CreateSave(const int SaveIndex)
{
	SaveT* NewSave = Cast<SaveT>(UGameplayStatics::CreateSaveGameObject(SaveT::StaticClass()));
	NewSave->SetSaveIndex(SaveIndex);
	NewSave = Cast<SaveT>(UENTSaveSubsystem::SaveToSlot(NewSave, SaveIndex));

	return NewSave;
}

/**
 * @brief Save to a slot
 * @tparam SaveT The type of save you want to load
 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
 * @param bCreateNewSaveIfDoesntExist his will create a new save if the one you're trying to load doesn't exist
 * @return The save object
 */
template<typename SaveT = UENTDefaultSave>
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
