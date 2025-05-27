// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PlayerEditorSettings.generated.h"

UCLASS(Config = EditorPerProjectUserSettings, EditorConfig = Editor)
class PRFEDITOR_API UPlayerEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/**
	 * @brief Is editor only, in packaged game it will be always true
	 */
	UPROPERTY(EditDefaultsOnly, Config, Category = "PlayerSettings")
	bool bDisplayStartWidget = true;
};
