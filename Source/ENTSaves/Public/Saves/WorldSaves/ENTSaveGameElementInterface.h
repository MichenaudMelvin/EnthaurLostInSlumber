// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTGameElementData.h"
#include "UObject/Interface.h"
#include "ENTSaveGameElementInterface.generated.h"

class UENTWorldSave;

// This class does not need to be modified.
UINTERFACE()
class ENTSAVES_API UENTSaveGameElementInterface : public UInterface
{
	GENERATED_BODY()
};

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
	 * @brief Implement this function to an actor
	 * @param GameElementData 
	 * @param LoadedWorldSave Ptr can be nullptr do a check before using it
	 */
	virtual void LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave) = 0;
};
