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

	UPROPERTY(BlueprintReadOnly, Category = "World")
	TArray<FName> SublevelsNames;

	virtual FString GetSlotName() const override {return WorldName + SlotName;}

#pragma region MapData

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FGuid, FENTMuscleData> MuscleData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FGuid, FENTNerveData> NerveData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FGuid, FENTAmberOreData> AmberOreData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FGuid, FENTWeakZoneData> WeakZoneData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FGuid, FENTRespawnTreeData> RespawnTreeData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FGuid, FENTSpikeDoorData> SpikeDoorData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FGuid, FENTParasiteData> ParasiteData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FGuid, FETNScriptedAIElementData> ScriptedAIElementsData;

	UPROPERTY(BlueprintReadWrite, Category = "World|GameElements")
	TMap<FGuid, FENTGameElementData> OtherElements;

#pragma endregion

	UPROPERTY(BlueprintReadWrite, Category = "World|Player")
	FVector PlayerLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "World|Player")
	FRotator PlayerCameraRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, Category = "World|Player")
	FString LastCheckPointName;

	void ClearMapData();
};
