// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultSave.h"
#include "WorldSave.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UWorldSave : public UDefaultSave
{
	GENERATED_BODY()

public:
	UWorldSave();

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	FTransform PlayerTransform = FTransform::Identity;
};
