// Fill out your copyright notice in the Description page of Project Settings.


#include "Toolbar/ToolbarModule.h"
#include "EditorSettings/PlayerEditorSettings.h"
#include "EditorSettings/SavesSettings.h"

IMPLEMENT_MODULE(UToolbarModule, ToolbarModule)

#define LOCTEXT_NAMESPACE "ToolbarModule"

void UToolbarModule::StartupModule()
{
	IModuleInterface::StartupModule();

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &UToolbarModule::RegisterMenuExtensions));
}

void UToolbarModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();

	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
}

void UToolbarModule::RegisterMenuExtensions()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection("Play");

	InitLoadLatestSaveCheckBox(ToolbarSection);
	InitPlayerEditorSettingsCheckBoxes(ToolbarSection);
}

void UToolbarModule::InitLoadLatestSaveCheckBox(FToolMenuSection& ToolbarSection)
{
	const TSharedRef<SCheckBox> CheckBox = SNew(SCheckBox).OnCheckStateChanged_Raw(this, &UToolbarModule::UpdateSaveSettings);
	CheckBox->SetToolTipText(FText::AsCultureInvariant("Load latest save in the current world if exist"));

	const USavesSettings* SavesSettings = GetDefault<USavesSettings>();
	CheckBox->SetIsChecked(SavesSettings->bLoadLatestWorldSave ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);

	const TSharedRef<SCenteredText> TextBlock = SNew(SCenteredText);
	TextBlock->SetText(NSLOCTEXT("ToolbarModule", "LoadLatestSave", "Load Latest Save"));

	ToolbarSection.AddSeparator("SaveSettingsSeparator");

	FString CheckBoxString = FString::Printf(TEXT("LoadLatestSaveCheckBox"));
	FName CheckBoxName = FName(*CheckBoxString);
	ToolbarSection.AddEntry(FToolMenuEntry::InitWidget(CheckBoxName, CheckBox, FText::FromString(CheckBoxString)));

	FString TextBlockString = FString::Printf(TEXT("LoadLatestSaveTextBlock"));
	FName TextBlockName = FName(*TextBlockString);
	ToolbarSection.AddEntry(FToolMenuEntry::InitWidget(TextBlockName, TextBlock, FText::FromString(TextBlockString)));
}

void UToolbarModule::InitPlayerEditorSettingsCheckBoxes(FToolMenuSection& ToolbarSection)
{
	const TSharedRef<SCheckBox> CheckBox = SNew(SCheckBox).OnCheckStateChanged_Raw(this, &UToolbarModule::UpdateDisplayStartWidgetSetting);
	CheckBox->SetToolTipText(FText::AsCultureInvariant("Will display the start widget and lock controls or not, In packaged game it will be always true"));

	const UPlayerEditorSettings* PlayerEditorSettings = GetDefault<UPlayerEditorSettings>();
	CheckBox->SetIsChecked(PlayerEditorSettings->bDisplayStartWidget ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);

	const TSharedRef<SCenteredText> TextBlock = SNew(SCenteredText);
	TextBlock->SetText(NSLOCTEXT("ToolbarModule", "DisplayStartWidget", "Display Start Widget"));

	ToolbarSection.AddSeparator("PlayerSettingsSeparator");

	FString CheckBoxString = FString::Printf(TEXT("DisplayStartWidgetCheckBox"));
	FName CheckBoxName = FName(*CheckBoxString);
	ToolbarSection.AddEntry(FToolMenuEntry::InitWidget(CheckBoxName, CheckBox, FText::FromString(CheckBoxString)));

	FString TextBlockString = FString::Printf(TEXT("DisplayStartWidgetTextBlock"));
	FName TextBlockName = FName(*TextBlockString);
	ToolbarSection.AddEntry(FToolMenuEntry::InitWidget(TextBlockName, TextBlock, FText::FromString(TextBlockString)));
}

void UToolbarModule::UpdateSaveSettings(ECheckBoxState CheckBoxState)
{
	USavesSettings* SavesSettings = GetMutableDefault<USavesSettings>();

	switch (CheckBoxState) {
	case ECheckBoxState::Unchecked:
	case ECheckBoxState::Undetermined:
		SavesSettings->bLoadLatestWorldSave = false;
		break;
	case ECheckBoxState::Checked:
		SavesSettings->bLoadLatestWorldSave = true;
		break;
	}

	SavesSettings->SaveConfig();
}

void UToolbarModule::UpdateDisplayStartWidgetSetting(ECheckBoxState CheckBoxState)
{
	UPlayerEditorSettings* PlayerEditorSettings = GetMutableDefault<UPlayerEditorSettings>();

	switch (CheckBoxState) {
	case ECheckBoxState::Unchecked:
	case ECheckBoxState::Undetermined:
		PlayerEditorSettings->bDisplayStartWidget = false;
		break;
	case ECheckBoxState::Checked:
		PlayerEditorSettings->bDisplayStartWidget = true;
		break;
	}

	PlayerEditorSettings->SaveConfig();
}

#undef LOCTEXT_NAMESPACE