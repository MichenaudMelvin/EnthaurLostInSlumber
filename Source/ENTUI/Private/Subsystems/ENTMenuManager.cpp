// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ENTMenuManager.h"
#include "EnhancedInputSubsystems.h"
#include "Config/ENTUIConfig.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Player/ENTControllerMappingContext.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/ENTHUDManager.h"

void UENTMenuManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// OnWorldBeginPlay isn't called properly in packaged games at Initialize()
	PostWorldInitDelegateHandle = FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UENTMenuManager::OnNewWorldStarted);
}

void UENTMenuManager::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnPostWorldInitialization.Remove(PostWorldInitDelegateHandle);
}

void UENTMenuManager::CreateAllWidgets()
{
	GetWorld()->OnWorldBeginPlay.Remove(CreateWidgetsDelegate);

	const UENTUIConfig* UIConfig = GetDefault<UENTUIConfig>();
	if (!IsValid(UIConfig))
	{
		return;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	PressAnyMenu = CreateWidget(PlayerController, UIConfig->PressAnyMenuClass);
	MainMenu = CreateWidget(PlayerController, UIConfig->MainMenuClass);
	NewGameMenu = CreateWidget(PlayerController, UIConfig->NewGameMenuClass);
	OptionsMenu = CreateWidget(PlayerController, UIConfig->OptionsMenuClass);
	CreditsMenu = CreateWidget(PlayerController, UIConfig->CreditsMenuClass);
	QuitMenu = CreateWidget(PlayerController, UIConfig->QuitMenuClass);
	PauseMenu = CreateWidget(PlayerController, UIConfig->PauseMenuClass);
	ControlsMenu = CreateWidget(PlayerController, UIConfig->ControlsMenuClass);
	MainMenuConfirmationMenu = CreateWidget(PlayerController, UIConfig->MainMenuConfirmationMenuClass);
	RestartConfirmationMenu = CreateWidget(PlayerController, UIConfig->RestartConfirmationMenuClass);

	OnWidgetsCreated.Broadcast();

	AENTDefaultPlayerController* FirstPersonController = Cast<AENTDefaultPlayerController>(PlayerController);
	if (!IsValid(FirstPersonController))
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

	CurrentState = EENTMenuState::Gameplay;
	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(Cast<IENTControllerMappingContext>(FirstPersonController)->GetDefaultMappingContext(), 0);
	SetGameInputMode();
	FirstPersonController->SetPause(false);

	FirstPersonController->OnPauseGame.AddDynamic(this, &UENTMenuManager::DisplayPauseMenu);
	FirstPersonController->OnNavigate; // No uses for this event right now
	FirstPersonController->OnSelect; // No uses for this event right now
	FirstPersonController->OnBack.AddDynamic(this, &UENTMenuManager::CloseCurrentMenu);
	FirstPersonController->OnResume.AddDynamic(this, &UENTMenuManager::Resume);
	FirstPersonController->ClearPlayerInputs();
}

void UENTMenuManager::OnNewWorldStarted(UWorld* World, FWorldInitializationValues WorldInitializationValues)
{
	if (!World)
	{
		return;
	}

	CreateWidgetsDelegate = World->OnWorldBeginPlay.AddUObject(this, &UENTMenuManager::CreateAllWidgets);
}

void UENTMenuManager::OpenMenu(UUserWidget* InMenuClass, bool bIsSubMenu)
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
}

void UENTMenuManager::CloseCurrentMenu()
{
	if (MenuStack.Num() == 0)
	{
		return;
	}

	// If we are in the main menu then we don't want to remove the first UI as we always need the "press any key"
	if (MenuStack.Num() == 1 && CurrentState == EENTMenuState::AnyMenu)
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

	if (MenuStack.Num() >= 1 && CurrentState != EENTMenuState::Gameplay)
	{
		TWeakObjectPtr<UUserWidget> NewTopMenuPtr = MenuStack.Last();
		if (!NewTopMenuPtr.IsValid())
		{
			return;
		}

		UUserWidget* NewMenu = NewTopMenuPtr.Get();
		NewMenu->AddToViewport();
	}
}

void UENTMenuManager::CloseAllMenus(EENTMenuState InState)
{
	for (int i = 0; i < MenuClasses.Num(); ++i)
	{
		CloseCurrentMenu();
		--i;
	}

	CurrentState = InState;

	if (CurrentState != EENTMenuState::Gameplay)
	{
		return;
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	UENTHUDManager* HUDManager = GameInstance->GetSubsystem<UENTHUDManager>();
	if (!HUDManager)
	{
		return;
	}

	HUDManager->SetHUDVisibility(ESlateVisibility::Visible);
}

void UENTMenuManager::SetMenuState(EENTMenuState InUIState)
{
	CurrentState = InUIState;
}

void UENTMenuManager::CheckMenuState()
{
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

	if (!Controller->Implements<UENTControllerMappingContext>())
	{
		return;
	}

	if (CurrentState == EENTMenuState::Waiting && !MenuStack.IsEmpty())
	{
		return;
	}
	
	if (MenuStack.Num() >= 2)
	{
		if (CurrentState == EENTMenuState::AnyMenu)
		{
			CurrentState = EENTMenuState::MainMenu;
			InputSubsystem->ClearAllMappings();
			InputSubsystem->AddMappingContext(Cast<IENTControllerMappingContext>(Controller)->GetUIMappingContext(), 0);
			SetUIInputMode();
		}
	}
	else if (MenuStack.Num() == 1 && CurrentState == EENTMenuState::MainMenu)
	{
		CurrentState = EENTMenuState::AnyMenu;
		InputSubsystem->ClearAllMappings();
		InputSubsystem->AddMappingContext(Cast<IENTControllerMappingContext>(Controller)->GetAnyKeyMappingContext(), 0);
		SetUIInputMode();
		Controller->SetPause(false);
	}
	else if (MenuStack.Num() >= 1 && CurrentState == EENTMenuState::Gameplay)
	{
		CurrentState = EENTMenuState::PauseMenu;
		InputSubsystem->ClearAllMappings();
		InputSubsystem->AddMappingContext(Cast<IENTControllerMappingContext>(Controller)->GetUIMappingContext(), 0);
		SetUIInputMode();
		CenterCursor();
		Controller->SetPause(true);
	}
	else if (MenuStack.IsEmpty())
	{
		CurrentState = EENTMenuState::Gameplay;
		InputSubsystem->ClearAllMappings();
		InputSubsystem->AddMappingContext(Cast<IENTControllerMappingContext>(Controller)->GetDefaultMappingContext(), 0);
		SetGameInputMode();
		Controller->SetPause(false);

		AENTDefaultPlayerController* FirstPersonController = Cast<AENTDefaultPlayerController>(Controller);
		if (!IsValid(FirstPersonController))
		{
			return;
		}

		FirstPersonController->ClearPlayerInputs();
	}
}

void UENTMenuManager::SetUIInputMode() const
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

void UENTMenuManager::SetGameInputMode() const
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

void UENTMenuManager::CenterCursor() const
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

void UENTMenuManager::DisplayPauseMenu()
{
	if (CurrentState != EENTMenuState::Gameplay)
	{
		return;
	}

	//UIManager->SetMenuState(EPRFUIState::PauseMenu);
	OpenMenu(PauseMenu, false);

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	UENTHUDManager* HUDManager = GameInstance->GetSubsystem<UENTHUDManager>();
	if (!HUDManager)
	{
		return;
	}

	HUDManager->SetHUDVisibility(ESlateVisibility::Hidden);
}

void UENTMenuManager::Resume()
{
	switch (CurrentState)
	{
	case EENTMenuState::PauseMenu:
		//UIManager->SetMenuState(EPRFUIState::Gameplay);
		CloseAllMenus(EENTMenuState::Gameplay);
		break;

	case EENTMenuState::MainMenu:
		CloseCurrentMenu();
		break;

	case EENTMenuState::AnyMenu:
	case EENTMenuState::Gameplay:
	case EENTMenuState::Waiting:
		break;
	}
}