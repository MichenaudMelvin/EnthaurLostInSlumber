// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTGameElementData.h"
#include "Saves/ENTDefaultSave.h"
#include "ENTWorldSave.generated.h"

UCLASS()
class ENTSAVES_API UENTWorldSave : public UENTDefaultSave
{
	GENERATED_BODY()

public:
	UENTWorldSave();

	UPROPERTY(BlueprintReadOnly, Category = "World")
	FString WorldName;

	virtual FString GetSlotName() const override {return WorldName + SlotName;}

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FString, FENTMuscleData> MuscleData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FString, FENTNerveData> NerveData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FString, FENTAmberOreData> AmberOreData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FString, FENTWeakZoneData> WeakZoneData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FString, FENTRespawnTreeData> RespawnTreeData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FString, FENTParaSiteData> ParasiteData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	FString LastCheckPointName;

	void ClearMapData();
};
