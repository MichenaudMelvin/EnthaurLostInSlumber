// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTClassValidator.h"
#include "ENTPathValidator.generated.h"

UCLASS(Abstract)
class ENTEDITOR_API UENTPathValidator : public UENTClassValidator
{
	GENERATED_BODY()

protected:
	virtual EDataValidationResult ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context) override;

	/**
	 * @brief Should start by /Game/*
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Path")
	TArray<FDirectoryPath> ValidPaths;

	/**
	 * @brief Will move the asset into the correct folder if invalid (one valid path only)
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Asset Validation")
	bool bMoveIfInvalid = true;
};
