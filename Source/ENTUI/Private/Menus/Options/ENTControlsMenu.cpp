// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Options/ENTControlsMenu.h"

#include "EnhancedInputSubsystems.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/LocalPlayer.h"
#include "Menus/Elements/ENTInputSlot.h"
#include "Subsystems/ENTMenuManager.h"
#include "UserSettings/EnhancedInputUserSettings.h"

void UENTControlsMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	AddInputRows();
}

void UENTControlsMenu::NativeDestruct()
{
	Super::NativeDestruct();
}

void UENTControlsMenu::AddInputRows()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		return;
	}

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!InputLocalPlayerSubsystem)
	{
		return;
	}

	UEnhancedInputUserSettings* EnhancedInputUserSettings = InputLocalPlayerSubsystem->GetUserSettings();
	if (!EnhancedInputUserSettings)
	{
		return;
	}
	
	UEnhancedPlayerMappableKeyProfile* PlayerMappableKeyProfile = EnhancedInputUserSettings->GetActiveKeyProfile();
	if (!IsValid(PlayerMappableKeyProfile))
	{
		return;
	}
	const TMap<FName, FKeyMappingRow>& PlayerMappingRows = PlayerMappableKeyProfile->GetPlayerMappingRows();

	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}

	for (const TTuple<FName, FKeyMappingRow>& Key : PlayerMappingRows)
	{
		UUserWidget* UserWidget = CreateWidget(PlayerController, InputSlotClass);
		UENTInputSlot* InputSlot = Cast<UENTInputSlot>(UserWidget);
		VBox->AddChild(InputSlot);

		FMapPlayerKeyArgs InArgs;
		InArgs.MappingName = Key.Key;
		InArgs.Slot = EPlayerMappableKeySlot::First;
		
		const FPlayerKeyMapping* PlayerKeyMapping = PlayerMappableKeyProfile->FindKeyMapping(InArgs);
		if (!PlayerKeyMapping)
		{
			continue;
		}

		FText KeyName = FText::AsCultureInvariant(PlayerKeyMapping->GetMappingName().ToString());
		InputSlot->SetKeyName(KeyName);
		InputSlot->SetButtonTextBlock(PlayerKeyMapping->GetDisplayName());
		InputSlot->SetKeyMappingName(Key.Key);

		UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(InputSlot->Slot);
		if (!IsValid(VerticalBoxSlot))
		{
			continue;
		}

		FSlateChildSize ChildSize;
		ChildSize.Value = ESlateSizeRule::Fill;
		
		VerticalBoxSlot->SetSize(ChildSize);
	}
}
