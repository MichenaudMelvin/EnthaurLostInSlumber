// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTGameElementData.h"
#include "UObject/Interface.h"
#include "ENTSaveGameElementInterface.generated.h"

class UENTWorldSave;

// This class does not need to be modified.
UINTERFACE()
class UENTSaveGameElementInterface : public UInterface
{
	GENERATED_BODY()
};

class ENTSAVES_API IENTSaveGameElementInterface
{
	GENERATED_BODY()

public:
	/**
	 * @brief Implement this function to an actor
	 * @param CurrentWorldSave 
	 * @return The return type can be used for inheritance methods
	 */
	virtual FENTGameElementData& SaveGameElement(UENTWorldSave* CurrentWorldSave) = 0;

	virtual void LoadGameElement(const FENTGameElementData& GameElementData) = 0;
};
