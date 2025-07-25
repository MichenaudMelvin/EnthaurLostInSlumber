// Fill out your copyright notice in the Description page of Project Settings.


#include "Toolbar/ENTToolbarModule.h"
#include "EditorSettings/ENTEditorSettings.h"

IMPLEMENT_MODULE(UENTToolbarModule, ENTToolbarModule)

#define LOCTEXT_NAMESPACE "ENTToolbarModule"

void UENTToolbarModule::StartupModule()
{
	IModuleInterface::StartupModule();

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &UENTToolbarModule::RegisterMenuExtensions));
}

void UENTToolbarModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();

	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
}

void UENTToolbarModule::RegisterMenuExtensions()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection("Play");

	InitLoadLatestSaveCheckBox(ToolbarSection);
	InitPlayerEditorSettingsCheckBoxes(ToolbarSection);
}

void UENTToolbarModule::InitLoadLatestSaveCheckBox(FToolMenuSection& ToolbarSection)
{
	const TSharedRef<SCheckBox> CheckBox = SNew(SCheckBox).OnCheckStateChanged_Raw(this, &UENTToolbarModule::UpdateSaveSettings);
	CheckBox->SetToolTipText(FText::AsCultureInvariant("Load latest save in the current world if exist"));

	const UENTEditorSettings* EditorSettings = GetDefault<UENTEditorSettings>();
	CheckBox->SetIsChecked(EditorSettings->bLoadLatestWorldSave ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);

	const TSharedRef<SENTCenteredText> TextBlock = SNew(SENTCenteredText);
	TextBlock->SetText(NSLOCTEXT("ToolbarModule", "LoadLatestSave", "Load Latest Save"));

	ToolbarSection.AddSeparator("SaveSettingsSeparator");

	FString CheckBoxString = FString::Printf(TEXT("LoadLatestSaveCheckBox"));
	FName CheckBoxName = FName(*CheckBoxString);
	ToolbarSection.AddEntry(FToolMenuEntry::InitWidget(CheckBoxName, CheckBox, FText::FromString(CheckBoxString)));

	FString TextBlockString = FString::Printf(TEXT("LoadLatestSaveTextBlock"));
	FName TextBlockName = FName(*TextBlockString);
	ToolbarSection.AddEntry(FToolMenuEntry::InitWidget(TextBlockName, TextBlock, FText::FromString(TextBlockString)));
}

void UENTToolbarModule::InitPlayerEditorSettingsCheckBoxes(FToolMenuSection& ToolbarSection)
{
	const TSharedRef<SCheckBox> CheckBox = SNew(SCheckBox).OnCheckStateChanged_Raw(this, &UENTToolbarModule::UpdateDisplayStartWidgetSetting);
	CheckBox->SetToolTipText(FText::AsCultureInvariant("Will display the start widget and lock controls or not, In packaged game it will be always true"));

	const UENTEditorSettings* EditorSettings = GetDefault<UENTEditorSettings>();
	CheckBox->SetIsChecked(EditorSettings->bDisplayStartWidget ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);

	const TSharedRef<SENTCenteredText> TextBlock = SNew(SENTCenteredText);
	TextBlock->SetText(NSLOCTEXT("ToolbarModule", "DisplayStartWidget", "Display Start Widget"));

	ToolbarSection.AddSeparator("PlayerSettingsSeparator");

	FString CheckBoxString = FString::Printf(TEXT("DisplayStartWidgetCheckBox"));
	FName CheckBoxName = FName(*CheckBoxString);
	ToolbarSection.AddEntry(FToolMenuEntry::InitWidget(CheckBoxName, CheckBox, FText::FromString(CheckBoxString)));

	FString TextBlockString = FString::Printf(TEXT("DisplayStartWidgetTextBlock"));
	FName TextBlockName = FName(*TextBlockString);
	ToolbarSection.AddEntry(FToolMenuEntry::InitWidget(TextBlockName, TextBlock, FText::FromString(TextBlockString)));
}

void UENTToolbarModule::UpdateSaveSettings(ECheckBoxState CheckBoxState)
{
	UENTEditorSettings* EditorSettings = GetMutableDefault<UENTEditorSettings>();

	switch (CheckBoxState) {
	case ECheckBoxState::Unchecked:
	case ECheckBoxState::Undetermined:
		EditorSettings->bLoadLatestWorldSave = false;
		break;
	case ECheckBoxState::Checked:
		EditorSettings->bLoadLatestWorldSave = true;
		break;
	}

	EditorSettings->SaveConfig();
}

void UENTToolbarModule::UpdateDisplayStartWidgetSetting(ECheckBoxState CheckBoxState)
{
	UENTEditorSettings* EditorSettings = GetMutableDefault<UENTEditorSettings>();

	switch (CheckBoxState) {
	case ECheckBoxState::Unchecked:
	case ECheckBoxState::Undetermined:
		EditorSettings->bDisplayStartWidget = false;
		break;
	case ECheckBoxState::Checked:
		EditorSettings->bDisplayStartWidget = true;
		break;
	}

	EditorSettings->SaveConfig();
}

#undef LOCTEXT_NAMESPACE