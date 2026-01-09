// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ENTSaveGameElementInterface.generated.h"

struct FENTGameElementData;
class UENTWorldSave;

// This class does not need to be modified.
UINTERFACE()
class ENTSAVES_API UENTSaveGameElementInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief Implement this interface to an actor class and create a struct derived from FENTGameElementData to save data 
 */
class ENTSAVES_API IENTSaveGameElementInterface
{
	GENERATED_BODY()

public:
	/**
	 * @brief Implement this function to an actor
	 * @param CurrentWorldSave Ptr will always be valid
	 * @return The return type can be used for inheritance methods
	 */
	virtual FENTGameElementData& SaveGameElement(UENTWorldSave* CurrentWorldSave) = 0;

	/**
	 * @brief Implement this function to an actor, This function can be called before the beginPlay
	 * @param GameElementData The Data of the loaded element, cast it to a specific type
	 * @param LoadedWorldSave Ptr can be nullptr do a check before using it
	 */
	virtual void LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave) = 0;

	/**
	 * @brief Implement this function to an actor, This function is called after begin loading
	 * @param LoadedWorldSave Ptr can be nullptr do a check before using it
	 */
	virtual void FinishLoading(UENTWorldSave* LoadedWorldSave) = 0;

	/**
	 * @brief Should return a Guid to identify the object when loading
	 * @return The Guid
	 */
	virtual const FGuid& GetSaveID() const = 0;

	/**
	 * @brief Implement this function to an actor
	 * @param CurrentWorldSave Ptr will always be valid
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Saves")
	void SaveGameElementBP(UENTWorldSave* CurrentWorldSave);

	/**
	 * @brief Implement this function to an actor, This function can be called before the beginPlay
	 * @param LoadedWorldSave Ptr can be nullptr do a check before using it
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Saves")
	void LoadGameElementBP(UENTWorldSave* LoadedWorldSave);

	/**
	 * @brief Implement this function to an actor, This function is called after begin loading
	 * @param LoadedWorldSave Ptr can be nullptr do a check before using it
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Saves")
	void FinishLoadingBP(UENTWorldSave* LoadedWorldSave);
};
