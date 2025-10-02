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
	//UE_LOG(LogTemp, Warning, TEXT("Mapping Name: %s"), *InArgs.MappingName.ToString());
	FGameplayTagContainer FailureReason;

	if (CheckDuplicateKeys(InKey))
	{
		return;
	}

	EnhancedInputUserSettings->MapPlayerKey(InArgs, FailureReason);
	if (!FailureReason.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("UENTControlsMenu::RebindKey - Failed to map player key: %s"), *FailureReason.ToString());
	}

	ActiveInputSlot->SetButtonKeyName(InKey.GetDisplayName());
}

bool UENTControlsMenu::CheckDuplicateKeys(const FKey& InKey)
{
	UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem = GetEnhancedInputLocalPlayerSubsystem();
	if (!InputLocalPlayerSubsystem)
	{
		return false;
	}

	UEnhancedInputUserSettings* EnhancedInputUserSettings = InputLocalPlayerSubsystem->GetUserSettings();
	if (!EnhancedInputUserSettings)
	{
		return false;
	}

	UEnhancedPlayerMappableKeyProfile* PlayerMappableKeyProfile = EnhancedInputUserSettings->GetActiveKeyProfile();
	if (!IsValid(PlayerMappableKeyProfile))
	{
		return false;
	}
	const TMap<FName, FKeyMappingRow>& PlayerMappingRows = PlayerMappableKeyProfile->GetPlayerMappingRows();

	for (const TTuple<FName, FKeyMappingRow>& Row : PlayerMappingRows)
	{
		const FKeyMappingRow& MappingRow = Row.Value;
		
		for (const FPlayerKeyMapping& Mapping : MappingRow.Mappings)
		{
			if (Mapping.GetCurrentKey() == InKey)
			{
				UE_LOG(LogTemp, Warning, TEXT("UENTControlsMenu::CheckDuplicateKeys - Key is already bound to an InputAction"));
				return true;
			}
		}
	}

	return false;
}

void UENTControlsMenu::OnKeyButton(UENTInputSlot* InInputSlot)
{
	ActiveInputSlot = InInputSlot;
}
