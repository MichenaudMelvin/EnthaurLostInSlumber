// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ENTGameElementData.generated.h"

USTRUCT(BlueprintType, Category = "SaveData")
struct ENTSAVES_API FENTGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	TArray<uint8> Bytes;
};

#pragma region GameElements

USTRUCT(BlueprintType, Category = "SaveData")
struct ENTSAVES_API FENTMuscleData : public FENTGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Muscle")
	bool bIsSolid = false;
};

USTRUCT(BlueprintType, Category = "SaveData")
struct ENTSAVES_API FENTNerveData : public FENTGameElementData
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
struct ENTSAVES_API FENTAmberOreData : public FENTGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "AmberOre")
	bool bIsEmpty = false;
};

USTRUCT(BlueprintType, Category = "SaveData")
struct ENTSAVES_API FENTWeakZoneData : public FENTGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "WeakZone")
	bool bIsActive = true;
};

USTRUCT(BlueprintType)
struct ENTSAVES_API FENTRespawnTreeData : public FENTGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "RespawnTree")
	bool bIsActive = false;
};

#pragma endregion

#pragma region AI

USTRUCT(BlueprintType, Category = "SaveData")
struct ENTSAVES_API FENTAIData : public FENTGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	bool bRunningBehaviorTree = false;

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	FTransform PawnTransform;
};

USTRUCT(BlueprintType, Category = "SaveData")
struct ENTSAVES_API FENTParasiteData : public FENTAIData
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

	UPROPERTY(BlueprintReadOnly, Category = "AI|Blackboard")
	FVector NoiseLocation = FVector::ZeroVector;
};

USTRUCT(BlueprintType, Category = "SaveData")
struct ENTSAVES_API FETNScriptedAIElementData : public FENTGameElementData
{
	GENERATED_BODY()
};

#pragma endregion