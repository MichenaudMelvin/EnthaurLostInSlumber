// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Saves/DefaultSave.h"
#include "WorldSave.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UWorldSave : public UDefaultSave
{
	GENERATED_BODY()

public:
	UWorldSave();

	UPROPERTY(BlueprintReadOnly, Category = "World")
	FString WorldName;

	virtual FString GetSlotName() const override {return WorldName + SlotName;}
};
