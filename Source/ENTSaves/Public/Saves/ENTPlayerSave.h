// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTDefaultSave.h"
#include "ENTPlayerSave.generated.h"

UCLASS()
class ENTSAVES_API UENTPlayerSave : public UENTDefaultSave
{
	GENERATED_BODY()

public:
	UENTPlayerSave();

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	FVector PlayerLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	FRotator PlayerCameraRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	FName LastWorldSaved = NAME_None;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	uint8 CurrentState = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Amber")
	TMap<uint8, int> AmberInventory;
};
