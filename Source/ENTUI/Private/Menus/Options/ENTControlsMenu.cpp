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

TObjectPtr<UEnhancedInputLocalPlayerSubsystem> UENTControlsMenu::GetEnhancedInputLocalPlayerSubsystem()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		return nullptr;
	}

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		return nullptr;
	}

	UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!InputLocalPlayerSubsystem)
	{
		return nullptr;
	}

	return InputLocalPlayerSubsystem;
}

void UENTControlsMenu::AddInputRows()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		return;
	}
	
	UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem = GetEnhancedInputLocalPlayerSubsystem();
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

		FText KeyName = FText::AsCultureInvariant(PlayerKeyMapping->GetCurrentKey().ToString());
		InputSlot->SetKeyName(PlayerKeyMapping->GetDisplayName());
		InputSlot->SetButtonKeyName(KeyName);
		InputSlot->SetKeyMappingName(Key.Key);
		InputSlot->SetControlsMenu(this);

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

void UENTControlsMenu::RebindKey(const FKey& InKey)
{
	UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem = GetEnhancedInputLocalPlayerSubsystem();
	if (!InputLocalPlayerSubsystem)
	{
		return;
	}

	UEnhancedInputUserSettings* EnhancedInputUserSettings = InputLocalPlayerSubsystem->GetUserSettings();
	if (!EnhancedInputUserSettings)
	{
		return;
	}

	FMapPlayerKeyArgs InArgs;
	InArgs.NewKey = InKey;
	InArgs.Slot = EPlayerMappableKeySlot::First;
	InArgs.MappingName = ActiveInputSlot->GetMappingName();
	UE_LOG(LogTemp, Warning, TEXT("Mapping Name: %s"), *InArgs.MappingName.ToString());

	FGameplayTagContainer FailureReason;

	EnhancedInputUserSettings->MapPlayerKey(InArgs, FailureReason);
	
	if (!FailureReason.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to map player key: %s"), *FailureReason.ToStringSimple());
	}

	ActiveInputSlot->SetButtonKeyName(InKey.GetDisplayName());
}

void UENTControlsMenu::OnKeyButton(UENTInputSlot* InInputSlot)
{
	ActiveInputSlot = InInputSlot;
}
