// Fill out your copyright notice in the Description page of Project Settings.


#include "Toolbar/ToolbarModule.h"
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
}

void UToolbarModule::InitLoadLatestSaveCheckBox(FToolMenuSection& ToolbarSection)
{
	const TSharedRef<SCheckBox> CheckBox = SNew(SCheckBox).OnCheckStateChanged_Raw(this, &UToolbarModule::UpdateSaveSettings);

	const USavesSettings* SavesSettings = GetDefault<USavesSettings>();
	CheckBox->SetIsChecked(SavesSettings->bLoadLatestWorldSave ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);

	const TSharedRef<STextBlock> TextBlock = SNew(STextBlock);
	TextBlock->SetText(FText::FromName("Load Latest Save"));

	ToolbarSection.AddSeparator("Separator");

	FString CheckBoxString = FString::Printf(TEXT("CheckBox"));
	FName CheckBoxName = FName(*CheckBoxString);
	ToolbarSection.AddEntry(FToolMenuEntry::InitWidget(CheckBoxName, CheckBox, FText::FromString(CheckBoxString)));

	FString TextBlockString = FString::Printf(TEXT("TextBlock"));
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

#undef LOCTEXT_NAMESPACE