// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Text/STextBlock.h"
#include "UObject/Object.h"

class PRFEDITOR_API SCenteredText : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCenteredText) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs) {}

	void SetText(const FText& Text)
	{
		ChildSlot
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(Text)
		];
	}
};

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
