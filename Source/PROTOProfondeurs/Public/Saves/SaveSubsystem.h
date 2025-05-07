// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultSave.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSubsystem.generated.h"

class UDefaultSave;

/**
 * @brief This subsystem is abstract
 */
UCLASS(Abstract, Category = "Save")
class PROTOPROFONDEURS_API USaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

#pragma region SaveFunctions

public:
	/**
	 * @brief Create a new save
	 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
	 * @return The created save
	 */
	UFUNCTION(BlueprintCallable, Category = "Saves", meta = (DisplayName = "CreateSave"))
	virtual void CreateSave(const int SaveIndex);

	/**
	 * @brief Save to a slot
	 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
	 * @return The save object
	 */
	UFUNCTION(BlueprintCallable, Category = "Saves")
	UDefaultSave* SaveToSlot(const int SaveIndex);

	/**
	 * @brief Load a save
	 * @param SaveIndex The save index; this is different from the user index and allow to create multiple saves with a same name
	 * @param bCreateNewSaveIfDoesntExist This will create a new save if the one you're trying to load doesn't exist
	 * @return The save object
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Saves", meta = (DisplayName = "LoadSave"))
	virtual UDefaultSave* LoadSave(const int SaveIndex, const bool bCreateNewSaveIfDoesntExist = true);

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
	TSubclassOf<UDefaultSave> SaveClass;

	UPROPERTY()
	TObjectPtr<UDefaultSave> SaveObject;
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
	NewSave = Cast<SaveT>(USaveSubsystem::SaveToSlot(NewSave, SaveIndex));

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
