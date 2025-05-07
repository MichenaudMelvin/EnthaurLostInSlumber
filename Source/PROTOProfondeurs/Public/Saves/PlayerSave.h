// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultSave.h"
#include "PlayerSave.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UPlayerSave : public UDefaultSave
{
	GENERATED_BODY()

public:
	UPlayerSave();

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	FTransform PlayerTransform = FTransform::Identity;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	FString LastWorldOpened;
};
