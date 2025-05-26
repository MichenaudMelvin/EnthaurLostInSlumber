// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelEnteringUI.generated.h"

/**
 * 
 */
UCLASS()
class PROTOPROFONDEURS_API ULevelEnteringUI : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> AreaName;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RegionName;
};
