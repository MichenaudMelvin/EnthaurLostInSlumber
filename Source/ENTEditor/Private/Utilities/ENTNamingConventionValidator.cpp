// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/ENTNamingConventionValidator.h"
#include "EditorUtilityLibrary.h"

EDataValidationResult UENTNamingConventionValidator::ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context)
{
	Super::ValidateLoadedAsset_Implementation(InAssetData, InAsset, Context);

	const FString AssetName = InAssetData.AssetName.ToString();

	for (const FString& Prefix : PossiblePrefixes)
	{
		if (AssetName.StartsWith(Prefix, ESearchCase::CaseSensitive))
		{
			AssetPasses(InAsset);
			return EDataValidationResult::Valid;
		}
	}

	FString AllPrefixes;
	FString Separator = ", ";
	for (const FString& Prefix : PossiblePrefixes)
	{
		AllPrefixes += Prefix + Separator;
	}

	FString Prefixes;
	FString UnusedStr;
	AllPrefixes.Split(Separator, &Prefixes, &UnusedStr, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

	const FText Message = FText::AsCultureInvariant("Missing Prefix: " + Prefixes);

	if (bUseWarn)
	{
		AssetWarning(InAsset, Message);
	}
	else
	{
		AssetFails(InAsset, Message);
	}

	if (bRenameIfInvalid && PossiblePrefixes.Num() == 1)
	{
		FString NewName = PossiblePrefixes.Array()[0] + AssetName; 
		UEditorUtilityLibrary::RenameAsset(InAsset, NewName);
	}

	return EDataValidationResult::Invalid;
}
