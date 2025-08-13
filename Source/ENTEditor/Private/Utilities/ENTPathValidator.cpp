// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/ENTPathValidator.h"
#include "EditorAssetLibrary.h"

EDataValidationResult UENTPathValidator::ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context)
{
	Super::ValidateLoadedAsset_Implementation(InAssetData, InAsset, Context);

	FString PathString = InAssetData.PackagePath.ToString();

	for (const FDirectoryPath& Path : ValidPaths)
	{
		if (PathString.StartsWith(Path.Path))
		{
			AssetPasses(InAsset);
			return EDataValidationResult::Valid;
		}
	}

	FString AllPaths;
	FString Separator = ", ";
	for (const FDirectoryPath& Path : ValidPaths)
	{
		AllPaths += Path.Path + Separator;
	}

	FString Paths;
	FString UnusedStr;
	AllPaths.Split(Separator, &Paths, &UnusedStr, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

	const FText Message = FText::AsCultureInvariant("Wrong path, please use " + Paths);

	AssetFails(InAsset, Message);

	if (bMoveIfInvalid && ValidPaths.Num() == 1)
	{
		UEditorAssetLibrary::RenameLoadedAsset(InAsset, ValidPaths[0].Path);
	}

	return EDataValidationResult::Invalid;
}
