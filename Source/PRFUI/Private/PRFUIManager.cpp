// Fill out your copyright notice in the Description page of Project Settings.


#include "PRFUIManager.h"

#include "UIManagerSettings.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void UPRFUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UUIManagerSettings* UIManagerSettings = GetDefault<UUIManagerSettings>();
	if (!IsValid(UIManagerSettings))
	{
		return;
	}
	
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
	SetUIInputMode();

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
		// Switch input mapping input context to UI
		
	}
	
	if (GEngine)
	{
		for (const TPair<FString, TWeakObjectPtr<UUserWidget>>& Pair : MenuClasses)
		{
			const FString& YoMenuName = Pair.Key;
			const TWeakObjectPtr<UUserWidget>& WidgetPtr = Pair.Value;

			FString Output;

			if (WidgetPtr.IsValid())
			{
				Output = FString::Printf(TEXT("Menu: %s → %s"), *YoMenuName, *WidgetPtr->GetName());
			}
			else
			{
				Output = FString::Printf(TEXT("Menu: %s → Invalid (GC'd or destroyed)"), *YoMenuName);
			}

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Output);
		}
	}
}

void UPRFUIManager::CloseCurrentMenu()
{
	if (MenuStack.Num() == 0)
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
		// Switch input mapping input context to Game
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

	//PlayerController->SetInputMode(FInputModeGameAndUI());
	UGameplayStatics::SetGamePaused(this, true);
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

	PlayerController->SetInputMode(FInputModeGameOnly());

	UGameplayStatics::SetGamePaused(this, false);
	PlayerController->bShowMouseCursor = false;
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
