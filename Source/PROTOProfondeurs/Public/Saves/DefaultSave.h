// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DefaultSave.generated.h"

/**
 * @brief The default save class, Don't forget to add assign SlotName in the ctor and put a UPROPERTY() on every member you want to serialize
 */
UCLASS(Abstract)
class PROTOPROFONDEURS_API UDefaultSave : public USaveGame{
	GENERATED_BODY()

protected:
	/**
	 * @brief The name of the save
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Save")
	FString SlotName;

	/**
	 * @brief The save index; this is different from the user index and allow to create multiple saves with a same name
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Save")
	int SaveIndex{0};

public:
	inline FString GetSlotName() const {return SlotName;}

	inline void SetSaveIndex(const int NewSaveIndex) {SaveIndex = NewSaveIndex;}

	inline int GetSaveIndex() const {return SaveIndex;}

#if WITH_EDITOR
	virtual void PrintVariables() const;
#endif
};