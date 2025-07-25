// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ENTEditorSettings.generated.h"

UCLASS(Config = EditorPerProjectUserSettings, EditorConfig = Editor, Category = "EnthaurSettings")
class ENTEDITOR_API UENTEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/**
	 * @brief Choose to display the level name or not, Editor only, in packaged game it will be always true
	 */
	UPROPERTY(EditDefaultsOnly, Config, Category = "PlayerSettings")
	bool bDisplayStartWidget = true;

	/**
	 * @brief Choose to load the latest save of the world, Editor Only, in packaged game it will be always true
	 */
	UPROPERTY(EditDefaultsOnly, Config, Category = "WorldSave")
	bool bLoadLatestWorldSave = false;
};
