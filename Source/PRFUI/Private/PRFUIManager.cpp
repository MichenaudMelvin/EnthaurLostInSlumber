// Fill out your copyright notice in the Description page of Project Settings.


#include "PRFUIManager.h"

#include "EnhancedInputSubsystems.h"
#include "UIManagerSettings.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Controller/PRFControllerMappingContext.h"
#include "Controller/PRFUIController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonController.h"


void UPRFUIManager::CreateAllWidgets()
{
	const UUIManagerSettings* UIManagerSettings = GetDefault<UUIManagerSettings>();
	if (!IsValid(UIManagerSettings))
	{
		return;
	}
	
	PressAnyMenu = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), UIManagerSettings->PressAnyMenuClass);
	MainMenu = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), UIManagerSettings->MainMenuClass);
	NewGameMenu = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), UIManagerSettings->NewGameMenuClass);
	OptionsMenu = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), UIManagerSettings->OptionsMenuClass);
	CreditsMenu = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), UIManagerSettings->CreditsMenuClass);
	QuitMenu = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), UIManagerSettings->QuitMenuClass);

	PauseMenu = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), UIManagerSettings->PauseMenuClass);

	OnWidgetsCreated.Broadcast();
}

void UPRFUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	CreateWidgetsDelegate = GetWorld()->OnWorldBeginPlay.AddUObject(this, &UPRFUIManager::CreateAllWidgets);
}

void UPRFUIManager::Deinitialize()
{
	Super::Deinitialize();

	GetWorld()->OnWorldBeginPlay.Remove(CreateWidgetsDelegate);
}

void UPRFUIManager::OpenMenu(UUserWidget* InMenuClass, bool bIsSubMenu)
{
	if (!IsValid(InMenuClass))
	{
		return;
	}

	const FString MenuKey = InMenuClass->GetClass()->GetName();
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

	CheckMenuState();
	
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::FromInt(MenuStack.Num()));
		
		for (const auto& Pair : MenuClasses)
		{
			const FString& YoMenuName = Pair.Key;
			const auto& WidgetPtr = Pair.Value;

			FString Output1;

			if (IsValid(WidgetPtr))
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

	FString test = FString::Printf(TEXT("%d"), CurrentState);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, test);*/
}

void UPRFUIManager::CloseCurrentMenu()
{
	if (MenuStack.Num() == 0)
	{
		return;
	}

	// If we are in the main menu then we don't want to remove the first UI as we always need the "press any key"
	if (MenuStack.Num() == 1 && CurrentState == EPRFUIState::AnyMenu)
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

	CheckMenuState();

	if (MenuStack.Num() >= 1 && CurrentState != EPRFUIState::Gameplay)
	{
		
		TWeakObjectPtr<UUserWidget> NewTopMenuPtr = MenuStack.Last();
		if (!TopMenuPtr.IsValid())
		{
			return;
		}
		
		UUserWidget* NewMenu = NewTopMenuPtr.Get();
		NewMenu->AddToViewport();
	}
	
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::FromInt(MenuStack.Num()));
		
		for (const auto& Pair : MenuClasses)
		{
			const FString& YoMenuName = Pair.Key;
			const auto& WidgetPtr = Pair.Value;

			FString Output1;

			if (IsValid(WidgetPtr))
			{
				Output1 = FString::Printf(TEXT("Menu: %s → %s"), *YoMenuName, *WidgetPtr->GetName());
			}
			else
			{
				Output1 = FString::Printf(TEXT("Menu: %s → Invalid (GC'd or destroyed)"), *YoMenuName);
			}

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Output1);
		}
	}

	FString test = FString::Printf(TEXT("%d"), CurrentState);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, test);*/
}

void UPRFUIManager::CloseAllMenus(EPRFUIState InState)
{
	for (int i = 0; i < MenuClasses.Num(); ++i)
	{
		CloseCurrentMenu();
		--i;
	}

	CurrentState = InState;
}

void UPRFUIManager::SetMenuState(EPRFUIState InUIState)
{
	CurrentState = InUIState;
}

void UPRFUIManager::CheckMenuState()
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

	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(Controller))
	{
		return;
	}

	if (!Controller->Implements<UPRFControllerMappingContext>())
	{
		return;
	}

	if (CurrentState == EPRFUIState::Waiting)
	{
		return;
	}
	
	if (MenuStack.Num() >= 2)
	{
		if (CurrentState == EPRFUIState::AnyMenu)
		{
			CurrentState = EPRFUIState::MainMenu;
			InputSubsystem->ClearAllMappings();
			InputSubsystem->AddMappingContext(Cast<IPRFControllerMappingContext>(Controller)->GetUIMappingContext(), 0);
			SetUIInputMode();
		}
	}
	else if (MenuStack.Num() == 1 && CurrentState == EPRFUIState::MainMenu)
	{
		CurrentState = EPRFUIState::AnyMenu;
		InputSubsystem->ClearAllMappings();
		InputSubsystem->AddMappingContext(Cast<IPRFControllerMappingContext>(Controller)->GetAnyKeyMappingContext(), 0);
		SetUIInputMode();
		Controller->SetPause(false);
	}
	else if (MenuStack.Num() >= 1 && CurrentState == EPRFUIState::Gameplay)
	{
		CurrentState = EPRFUIState::PauseMenu;
		InputSubsystem->ClearAllMappings();
		InputSubsystem->AddMappingContext(Cast<IPRFControllerMappingContext>(Controller)->GetUIMappingContext(), 0);
		SetUIInputMode();
		CenterCursor();
		Controller->SetPause(true);
	}
	else if (MenuStack.IsEmpty())
	{
		CurrentState = EPRFUIState::Gameplay;
		InputSubsystem->ClearAllMappings();
		InputSubsystem->AddMappingContext(Cast<IPRFControllerMappingContext>(Controller)->GetDefaultMappingContext(), 0);
		SetGameInputMode();
		Controller->SetPause(false);
		
		AFirstPersonController* FirstPersonController = Cast<AFirstPersonController>(Controller);
		if (!IsValid(FirstPersonController))
		{
			return;
		}
		
		FirstPersonController->ClearPlayerInputs();
	}
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
