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
	FName LastWorldSaved = NAME_None;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	uint8 CurrentState = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Amber")
	bool bHasAmber = false;

	UPROPERTY(BlueprintReadWrite, Category = "Lantern")
	bool bIsLanternActive = false;

	UPROPERTY(BlueprintReadWrite, Category = "Music")
	FName AkEventPath = NAME_None;

	UPROPERTY(BlueprintReadWrite, Category = "Music")
	FName AkSwitchPath = NAME_None;
};
