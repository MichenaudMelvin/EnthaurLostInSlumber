// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ENTMenuManager.generated.h"

UENUM(BlueprintType)
enum class EENTMenuState : uint8
{
	AnyMenu,
	MainMenu,
	Gameplay,
	PauseMenu,
	Waiting
};

UCLASS()
class ENTUI_API UENTMenuManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	// bIsSubMenu is for confirmation boxes that have to not hide the menu just before
	// Can also be used for menus with a fixed part and an inner part that changes (options menu)
	UFUNCTION(BlueprintCallable, Category = "UI")
	void OpenMenu(UUserWidget* InMenuClass, bool bIsSubMenu);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void CloseCurrentMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void CloseAllMenus(EENTMenuState InState);

	TObjectPtr<UUserWidget> GetPauseMenu() const { return PauseMenu; }
	TObjectPtr<UUserWidget> GetMainMenu() const { return MainMenu; }
	TObjectPtr<UUserWidget> GetOptionsMenu() const { return OptionsMenu; }
	TObjectPtr<UUserWidget> GetCreditsMenu() const { return CreditsMenu; }
	TObjectPtr<UUserWidget> GetQuitMenu() const { return QuitMenu; }
	TObjectPtr<UUserWidget> GetNewGameMenu() const { return NewGameMenu; }
	TObjectPtr<UUserWidget> GetControlsMenu() const { return ControlsMenu; }
	TObjectPtr<UUserWidget> GetMainMenuConfirmationMenu() const { return MainMenuConfirmationMenu; }
	TObjectPtr<UUserWidget> GetRestartConfirmationMenu() const { return RestartConfirmationMenu; }

	EENTMenuState GetMenuState() const { return CurrentState; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetMenuState(EENTMenuState InUIState);

	void CheckMenuState();

	void SetUIInputMode() const;
	void SetGameInputMode() const;

protected:
	void CreateAllWidgets();
	void CenterCursor() const;

	void OnNewWorldStarted(UWorld* World, FWorldInitializationValues WorldInitializationValues);

	FDelegateHandle CreateWidgetsDelegate;

	FDelegateHandle PostWorldInitDelegateHandle;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetsCreated);

	UPROPERTY(BlueprintAssignable)
	FOnWidgetsCreated OnWidgetsCreated;

#pragma region UI State

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	EENTMenuState CurrentState = EENTMenuState::AnyMenu;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnUIContextChanged, EENTMenuState);
	FOnUIContextChanged OnUIContextChanged;

#pragma endregion

#pragma region Main Menus

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI|PauseMenus")
	TObjectPtr<UUserWidget> PressAnyMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI|MainMenus")
	TObjectPtr<UUserWidget> MainMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI|MainMenus")
	TObjectPtr<UUserWidget> NewGameMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI|MainMenus")
	TObjectPtr<UUserWidget> LoadGameMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI|MainMenus")
	TObjectPtr<UUserWidget> CreditsMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI|MainMenus")
	TObjectPtr<UUserWidget> QuitMenu = nullptr;

#pragma endregion

#pragma region Pause Menus

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI|PauseMenus")
	TObjectPtr<UUserWidget> PauseMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI|PauseMenus")
	TObjectPtr<UUserWidget> OptionsMenu = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI|PauseMenus")
	TObjectPtr<UUserWidget> MainMenuConfirmationMenu = nullptr;

	TObjectPtr<UUserWidget> RestartConfirmationMenu = nullptr;

#pragma endregion

#pragma region Option Menus

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI|OptionMenus")
	TObjectPtr<UUserWidget> ControlsMenu = nullptr;

#pragma endregion

#pragma region Actions

protected:
	UFUNCTION()
	void DisplayPauseMenu();

	UFUNCTION()
	void Resume();

#pragma endregion

private:
	TArray<TWeakObjectPtr<UUserWidget>> MenuStack;
	TMap<FString, UUserWidget*> MenuClasses;
};
