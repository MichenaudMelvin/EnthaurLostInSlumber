// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "ENTClassValidator.generated.h"

UCLASS(Abstract)
class ENTEDITOR_API UENTClassValidator : public UEditorValidatorBase
{
	GENERATED_BODY()

protected:
	virtual bool CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const override;

	bool CheckClass(UObject* InObject, const TSubclassOf<UObject>& Class) const;

	/**
	 * @brief This includes child classes
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Class")
	TSet<TSubclassOf<UObject>> PossibleClasses;

	/**
	 * @brief This includes child classes
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Class")
	TSet<TSubclassOf<UObject>> ExcludedClasses;
};
