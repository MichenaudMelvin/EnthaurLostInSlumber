// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/ENTClassValidator.h"
#include "PluginBlueprintLibrary.h"

UENTClassValidator::UENTClassValidator()
{
	ExcludedPaths.Add(FDirectoryPath("/Engine"));
	ExcludedPaths.Add(FDirectoryPath("/Script"));
	ExcludedPaths.Add(FDirectoryPath("/Game/StarterContent"));
	ExcludedPaths.Add(FDirectoryPath("/Game/Trash"));
}

bool UENTClassValidator::CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const
{
	Super::CanValidateAsset_Implementation(InAssetData, InObject, InContext);

	FString PluginName;
	bool bIsAPluginAsset = UPluginBlueprintLibrary::GetPluginNameForObjectPath(InAssetData.GetSoftObjectPath(), PluginName);

	if (bIsAPluginAsset && !bAllowPluginCheck)
	{
		return false;
	}

	for (const FDirectoryPath& Path : ExcludedPaths)
	{
		if (InAssetData.PackagePath.ToString().StartsWith(Path.Path))
		{
			return false;
		}
	}

	for (const TSubclassOf<UObject>& ExcludedClass : ExcludedClasses)
	{
		if (CheckClass(InObject, ExcludedClass))
		{
			return false;
		}
	}

	for (const TSubclassOf<UObject>& PossibleClass : PossibleClasses)
	{
		if (CheckClass(InObject, PossibleClass))
		{
			return true;
		}
	}

	return false;
}

bool UENTClassValidator::CheckClass(UObject* InObject, const TSubclassOf<UObject>& Class) const
{
	bool bIsBlueprintClassMatching = false;
	if (Cast<UBlueprint>(InObject))
	{
		UBlueprint* Blueprint = Cast<UBlueprint>(InObject);
		TSubclassOf<UObject> BlueprintClass = Blueprint->ParentClass;
		bIsBlueprintClassMatching = BlueprintClass->IsChildOf(Class);
	}

	if (InObject->IsA(Class) || bIsBlueprintClassMatching)
	{
		return true;
	}

	return false;
}
