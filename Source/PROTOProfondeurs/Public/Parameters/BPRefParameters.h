// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BPRefParameters.generated.h"

USTRUCT(Blueprintable)
struct FDuoText
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText AreaZone;

	UPROPERTY(EditAnywhere)
	FText RegionZone;
};

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Blueprint References"))
class PROTOPROFONDEURS_API UBPRefParameters : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Config, Category = "WorldNames")
	TMap<TSoftObjectPtr<UWorld>, FDuoText> LevelNames;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Electricity")
	TSubclassOf<AActor> ElectricityFeedback;
};
