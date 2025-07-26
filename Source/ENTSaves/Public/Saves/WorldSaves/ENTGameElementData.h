// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ENTGameElementData.generated.h"

USTRUCT(BlueprintType, Category = "SaveData")
struct FENTGameElementData
{
	GENERATED_BODY()
};

#pragma region GameElements

USTRUCT(BlueprintType, Category = "SaveData")
struct FENTMuscleData : public FENTGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Muscle")
	bool bIsSolid = false;
};

USTRUCT(BlueprintType, Category = "SaveData")
struct FENTNerveData : public FENTGameElementData
{
	GENERATED_BODY()

	/**
	 * @brief In local space
	 */
	UPROPERTY(BlueprintReadWrite, Category= "Nerve")
	TArray<FVector> SplinePointsLocations;

	UPROPERTY(BlueprintReadWrite, Category= "Nerve")
	TArray<FVector> ImpactNormals;
};

USTRUCT(BlueprintType, Category = "SaveData")
struct FENTAmberOreData : public FENTGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "AmberOre")
	uint8 CurrentOreAmount = 0;
};

USTRUCT(BlueprintType, Category = "SaveData")
struct FENTWeakZoneData : public FENTGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "WeakZone")
	TArray<bool> ActivatedInteractionPoints;
};

USTRUCT(BlueprintType)
struct FENTRespawnTreeData : public FENTGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "RespawnTree")
	bool bIsActive = false;
};

#pragma endregion

#pragma region AI

USTRUCT(BlueprintType, Category = "SaveData")
struct FENTAIData : public FENTGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	FTransform PawnTransform;
};

USTRUCT(BlueprintType, Category = "SaveData")
struct FENTParaSiteData : public FENTAIData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AI|Blackboard")
	int PathIndex = -1;

	UPROPERTY(BlueprintReadOnly, Category = "AI|Blackboard")
	int PathDirection = 0;

	UPROPERTY(BlueprintReadOnly, Category = "AI|Blackboard")
	bool bWalkOnFloor = false;

	UPROPERTY(BlueprintReadOnly, Category = "AI|Blackboard")
	FVector MoveToLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "AI|Blackboard")
	bool bHeardNoise = false;
};

#pragma endregion