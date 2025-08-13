// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/ENTRedirectorValidator.h"

bool UENTRedirectorValidator::CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const
{
	Super::CanValidateAsset_Implementation(InAssetData, InObject, InContext);

	return InAssetData.IsRedirector();
}

EDataValidationResult UENTRedirectorValidator::ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context)
{
	Super::ValidateLoadedAsset_Implementation(InAssetData, InAsset, Context);

	if (InAssetData.IsRedirector())
	{
		AssetWarning(InAsset, FText::AsCultureInvariant("Redirectors should be deleted as soon as possible, make sure to check the object dependencies with the reference viewer before deleting it"));
		return EDataValidationResult::Invalid;
	}

	AssetPasses(InAsset);

	return EDataValidationResult::Valid;
}
