// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

class PRFEDITOR_API UToolbarModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

protected:
	void RegisterMenuExtensions();

	void InitLoadLatestSaveCheckBox(FToolMenuSection& ToolbarSection);

	void InitPlayerEditorSettingsCheckBoxes(FToolMenuSection& ToolbarSection);

	void UpdateSaveSettings(ECheckBoxState CheckBoxState);

	void UpdateDisplayStartWidgetSetting(ECheckBoxState CheckBoxState);
};
