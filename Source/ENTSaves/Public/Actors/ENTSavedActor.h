// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTSaveGameElementInterface.h"
#include "ENTSavedActor.generated.h"

UCLASS(Abstract)
class ENTSAVES_API AENTSavedActor : public AActor, public IENTSaveGameElementInterface
{
	GENERATED_BODY()

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Save")
	FGuid SaveID;

	FENTGameElementData EmptyData;

	// TODO this
	// bool bIsLoaded = false;

	virtual FENTGameElementData& SaveGameElement(UENTWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave) override;

	virtual void FinishLoading(UENTWorldSave* LoadedWorldSave) override;

	virtual const FGuid& GetSaveID() const override {return SaveID;}
};
