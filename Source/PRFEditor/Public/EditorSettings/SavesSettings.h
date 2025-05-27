// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SavesSettings.generated.h"

UCLASS(Config = EditorPerProjectUserSettings, EditorConfig = Editor)
class PRFEDITOR_API USavesSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Config, Category = "WorldSave")
	bool bLoadLatestWorldSave = false;
};
