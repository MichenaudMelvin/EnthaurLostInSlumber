// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelEnteringUI.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROTOPROFONDEURS_API ULevelEnteringUI : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> AreaNameCPP;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> RegionNameCPP;
	
};
