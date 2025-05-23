// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Saves/WorldSaves/SaveGameElementInterface.h"
#include "SaveAIData.generated.h"

USTRUCT(BlueprintType)
struct FAIData : public FGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	FTransform PawnTransform;
};