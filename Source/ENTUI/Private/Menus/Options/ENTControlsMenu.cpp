// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Options/ENTControlsMenu.h"

#include "EnhancedInputSubsystems.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/LocalPlayer.h"
#include "Menus/Elements/ENTCustomButton.h"
#include "Menus/Elements/ENTInputSlot.h"
#include "Menus/Options/ENTResetConfirmationMenu.h"
#include "Subsystems/ENTMenuManager.h"
#include "UserSettings/EnhancedInputUserSettings.h"

void UENTControlsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}

	UpdateAnyKeyBind(MenuManager->GetCurrentController());
	MenuManager->OnChangeCurrentController.AddDynamic(this, &UENTControlsMenu::UpdateAnyKeyBind);

	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTControlsMenu::OpenResetSettingsMenu);
	}
}

void UENTControlsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	VBox->ClearChildren();
	AddInputRows();
}

void UENTControlsMenu::NativeDestruct()
{
	Super::NativeDestruct();
}

void UENTControlsMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (ResetButton && ResetButton->GetCustomButton())
	{
		ResetButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTControlsMenu::OpenResetSettingsMenu);
	}
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

	for (FName MappingName : InputActionCustomDisplayOrder)
	{
		const FKeyMappingRow* PlayerMappingRow = PlayerMappingRows.Find(MappingName);
		if (!PlayerMappingRow)
		{
			continue;
		}

		UUserWidget* UserWidget = CreateWidget(PlayerController, InputSlotClass);
		UENTInputSlot* InputSlot = Cast<UENTInputSlot>(UserWidget);
		VBox->AddChild(InputSlot);

		FMapPlayerKeyArgs InArgs;
		InArgs.MappingName = MappingName;
		InArgs.Slot = EPlayerMappableKeySlot::First;

		const FPlayerKeyMapping* PlayerKeyMapping = PlayerMappableKeyProfile->FindKeyMapping(InArgs);
		if (!PlayerKeyMapping)
		{
			continue;
		}

		FText KeyName = FText::AsCultureInvariant(PlayerKeyMapping->GetCurrentKey().ToString());
		InputSlot->SetKeyName(PlayerKeyMapping->GetDisplayName());
		InputSlot->SetButtonKeyName(KeyName);
		InputSlot->SetKeyMappingName(MappingName);
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
	if (!ActiveInputSlot)
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
	EnhancedInputUserSettings->SaveSettings();

	ActiveInputSlot = nullptr;

	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}

	MenuManager->SetIsRebinding(false);
	VBox->ClearChildren();
	AddInputRows();
}

bool UENTControlsMenu::CheckDuplicateKeys(const FKey& InKey)
{
    UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem = GetEnhancedInputLocalPlayerSubsystem();
    if (!InputLocalPlayerSubsystem)
    {
	    return false;
    }

    UEnhancedInputUserSettings* UserSettings = InputLocalPlayerSubsystem->GetUserSettings();
    if (!UserSettings)
    {
	    return false;
    }

    UEnhancedPlayerMappableKeyProfile* Profile = UserSettings->GetActiveKeyProfile();
    if (!IsValid(Profile))
    {
	    return false;
    }

    const TMap<FName, FKeyMappingRow>& Rows = Profile->GetPlayerMappingRows();
    const FName CurrentMappingName = ActiveInputSlot->GetMappingName();

	FKey OldKey;
	{
    	const FKeyMappingRow* CurrentRow = Rows.Find(CurrentMappingName);
    	if (CurrentRow && CurrentRow->Mappings.Num() > 0)
    	{
    		const FPlayerKeyMapping& FirstMapping = *CurrentRow->Mappings.begin();
    		OldKey = FirstMapping.GetCurrentKey();
    	}
    	else
    	{
    		return false;
    	}
	}

    for (const auto& Pair : Rows)
    {
        const FName& OtherMappingName = Pair.Key;
        const FKeyMappingRow& Row = Pair.Value;

        for (const FPlayerKeyMapping& Mapping : Row.Mappings)
        {
            if (Mapping.GetCurrentKey() == InKey &&
                OtherMappingName != CurrentMappingName)
            {
                UE_LOG(LogTemp, Warning, TEXT("Duplicate detected. Swapping %s with %s"),*OldKey.ToString(), *InKey.ToString());

                FMapPlayerKeyArgs SwapOutArgs;
                SwapOutArgs.NewKey = OldKey;
                SwapOutArgs.Slot = EPlayerMappableKeySlot::First;
                SwapOutArgs.MappingName = OtherMappingName;

                FGameplayTagContainer SwapFailure;

                UserSettings->MapPlayerKey(SwapOutArgs, SwapFailure);

                if (!SwapFailure.IsEmpty())
                {
                    UE_LOG(LogTemp, Warning, TEXT("Swap failed: %s"), *SwapFailure.ToString());
                    return false;
                }

                return false;
            }
        }
    }

    return false;
}

void UENTControlsMenu::UpdateAnyKeyBind(AENTAnyKeyController* CurrentController)
{
	if (!CurrentController)
	{
		return;
	}

	CurrentController->OnAnyKeyPressed.AddDynamic(this, &UENTControlsMenu::RebindKey);
}

void UENTControlsMenu::OnKeyButton(UENTInputSlot* InInputSlot)
{
	ActiveInputSlot = InInputSlot;

	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}

	MenuManager->SetIsRebinding(true);
}

void UENTControlsMenu::ResetKeys()
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

	UEnhancedPlayerMappableKeyProfile* Profile = EnhancedInputUserSettings->GetActiveKeyProfile();

	Profile->ResetToDefault();
	EnhancedInputUserSettings->SaveSettings();
	VBox->ClearChildren();
	AddInputRows();

	UE_LOG(LogTemp, Warning, TEXT("Reset keys!"));
}

void UENTControlsMenu::OpenResetSettingsMenu()
{
	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}

	UENTResetConfirmationMenu* ResetConfirmationMenu = Cast<UENTResetConfirmationMenu>(MenuManager->GetResetConfirmationMenu());
	if (!IsValid(ResetConfirmationMenu))
	{
		return;
	}

	ResetConfirmationMenu->SetMenuType(EENTResetMenuType::Controls);
	MenuManager->OpenMenu(MenuManager->GetResetConfirmationMenu(), false);
}
