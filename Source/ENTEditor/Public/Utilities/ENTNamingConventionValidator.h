// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTClassValidator.h"
#include "ENTNamingConventionValidator.generated.h"

UCLASS(Abstract)
class ENTEDITOR_API UENTNamingConventionValidator : public UENTClassValidator
{
	GENERATED_BODY()

protected:
	virtual EDataValidationResult ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context) override;

	/**
	 * @brief If one of them is correct then the asset will be validated
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Prefix")
	TSet<FString> PossiblePrefixes;

	/**
	 * @brief Log a warning instead of an error if the asset doesn't respect the naming convention
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Severity")
	bool bUseWarn = true;

	/**
	 * @brief Will rename the asset if invalid (one possible prefix only)
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Asset Validation")
	bool bRenameIfInvalid = true;
};
