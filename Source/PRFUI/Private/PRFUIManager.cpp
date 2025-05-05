// Fill out your copyright notice in the Description page of Project Settings.


#include "PRFUIManager.h"

#include "EnhancedInputSubsystems.h"
#include "UIManagerSettings.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonController.h"

void UPRFUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UUIManagerSettings* UIManagerSettings = GetDefault<UUIManagerSettings>();
	if (!IsValid(UIManagerSettings))
	{
		return;
	}

	PressAnyMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->PressAnyMenuClass);
	MainMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->MainMenuClass);
	PauseMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->PauseMenuClass);
	OptionsMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->OptionsMenuClass);
}

void UPRFUIManager::OpenMenu(UUserWidget* InMenuClass, bool bIsSubMenu)
{
	if (!IsValid(InMenuClass))
	{
		return;
	}

	const FString MenuKey = InMenuClass->GetName();
	MenuClasses.Add(MenuKey, InMenuClass);
	MenuStack.Add(InMenuClass);

	InMenuClass->AddToViewport();

	if (MenuStack.Num() >= 2)
	{
		if (!bIsSubMenu)
		{
			TWeakObjectPtr<UUserWidget> PreviousMenu = MenuStack[MenuStack.Num() - 2];
			if (PreviousMenu.IsValid())
			{
				PreviousMenu->RemoveFromParent();
			}
		}
	}

	if (MenuStack.Num() == 1)
	{
		UWorld* World = GEngine->GetCurrentPlayWorld();
		if (!IsValid(World))
		{
			return;
		}
		
		ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
		if (!IsValid(LocalPlayer))
		{
			return;
		}
		
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (!IsValid(InputSubsystem))
		{
			return;
		}

		AFirstPersonController* FirstPersonController = Cast<AFirstPersonController>(UGameplayStatics::GetPlayerController(this, 0));
		if (!IsValid(FirstPersonController))
		{
			return;
		}

		InputSubsystem->ClearAllMappings();
		InputSubsystem->AddMappingContext(FirstPersonController->GetUIMappingContext(), 0);
		SetUIInputMode();
		CenterCursor();
	}
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::FromInt(MenuStack.Num()));
		
		for (const TPair<FString, TWeakObjectPtr<UUserWidget>>& Pair : MenuClasses)
		{
			const FString& YoMenuName = Pair.Key;
			const TWeakObjectPtr<UUserWidget>& WidgetPtr = Pair.Value;

			FString Output1;

			if (WidgetPtr.IsValid())
			{
				Output1 = FString::Printf(TEXT("Menu: %s → %s"), *YoMenuName, *WidgetPtr->GetName());
			}
			else
			{
				Output1 = FString::Printf(TEXT("Menu: %s → Invalid (GC'd or destroyed)"), *YoMenuName);
			}

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Output1);
		}
	}
}

void UPRFUIManager::CloseCurrentMenu()
{
	if (MenuStack.Num() == 0)
	{
		return;
	}

	// If we are in the main menu then we don't want to remove the first UI as we always need the "press any key"
	if (MenuStack.Num() == 1 && CurrentState == EPRFUIState::MainMenu)
	{
		return;
	}

	TWeakObjectPtr<UUserWidget> TopMenuPtr = MenuStack.Last();
	if (!TopMenuPtr.IsValid())
	{
		return;
	}
	
	MenuStack.Pop();

	UUserWidget* Menu = TopMenuPtr.Get();
	FString MenuKey = Menu->GetClass()->GetName();

	Menu->RemoveFromParent();
	MenuClasses.Remove(MenuKey);

	if (MenuStack.Num() == 0)
	{
		UWorld* World = GEngine->GetCurrentPlayWorld();
		if (!IsValid(World))
		{
			return;
		}
		
		ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
		if (!IsValid(LocalPlayer))
		{
			return;
		}
		
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (!IsValid(InputSubsystem))
		{
			return;
		}

		AFirstPersonController* FirstPersonController = Cast<AFirstPersonController>(UGameplayStatics::GetPlayerController(this, 0));
		if (!IsValid(FirstPersonController))
		{
			return;
		}

		InputSubsystem->ClearAllMappings();
		InputSubsystem->AddMappingContext(FirstPersonController->GetDefaultMappingContext(), 0);
		SetGameInputMode();
	}
}

void UPRFUIManager::CloseAllMenus()
{
	for (const TPair<FString, TWeakObjectPtr<UUserWidget>>& Pair : MenuClasses)
	{
		CloseCurrentMenu();
	}
}

void UPRFUIManager::SetMenuState(EPRFUIState InUIState)
{
	CurrentState = InUIState;
}

void UPRFUIManager::SetUIInputMode() const
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!IsValid(World))
	{
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		return;
	}

	UWidget* TestWidget = MenuStack.Last().Get();
	if (!IsValid(TestWidget))
	{
		return;
	}

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, MenuStack.Last().Get());
	//UGameplayStatics::SetGamePaused(this, true);
	PlayerController->bShowMouseCursor = true;
}

void UPRFUIManager::SetGameInputMode() const
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!IsValid(World))
	{
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		return;
	}

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
	//UGameplayStatics::SetGamePaused(this, false);
	PlayerController->bShowMouseCursor = false;
}

void UPRFUIManager::CenterCursor() const
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!IsValid(World))
	{
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		return;
	}

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	const int32 CenterX = FMath::RoundToInt(ViewportSize.X * 0.5f);
	const int32 CenterY = FMath::RoundToInt(ViewportSize.Y * 0.5f);

	PlayerController->SetMouseLocation(CenterX, CenterY);
}

void UPRFUIManager::HandleMenuCollection(UUserWidget* InMenuClass, bool bAddMenu)
{
	const FString MenuName = InMenuClass->GetName();
	
	if (bAddMenu)
	{
		MenuStack.Add(InMenuClass);
		MenuClasses.Add(MenuName, InMenuClass);
		InMenuClass->AddToViewport();
	}
	else
	{
		MenuStack.Remove(InMenuClass);
		MenuClasses.Remove(MenuName);
		InMenuClass->RemoveFromParent();
	}
}
