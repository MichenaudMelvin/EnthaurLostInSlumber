// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EPRFUIState.h"
#include "UObject/Object.h"
#include "PRFUIManager.generated.h"

class AFirstPersonController;

/**
 * 
 */
UCLASS()
class PRFUI_API UPRFUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// bIsSubMenu is for confirmation boxes that have to not hide the menu just before
	// Can also be used for menus with a fixed part and an inner part that changes (options menu)

	UFUNCTION(BlueprintCallable)
	void OpenMenu(UUserWidget* InMenuClass, bool bIsSubMenu);

	UFUNCTION(BlueprintCallable)
	void CloseCurrentMenu();
	
	UFUNCTION(BlueprintCallable)
	void CloseAllMenus(EPRFUIState InState);

	TObjectPtr<UUserWidget> GetPauseMenu() const { return PauseMenu; }
	TObjectPtr<UUserWidget> GetMainMenu() const { return MainMenu; }
	TObjectPtr<UUserWidget> GetOptionsMenu() const { return OptionsMenu; }
	TObjectPtr<UUserWidget> GetCreditsMenu() const { return CreditsMenu; }
	TObjectPtr<UUserWidget> GetQuitMenu() const { return QuitMenu; }
	TObjectPtr<UUserWidget> GetNewGameMenu() const { return NewGameMenu; }
	TObjectPtr<UUserWidget> GetControlsMenu() const { return ControlsMenu; }
	TObjectPtr<UUserWidget> GetMainMenuConfirmationMenu() const { return MainMenuConfirmationMenu; }
	TObjectPtr<UUserWidget> GetRestartConfirmationMenu() const { return RestartConfirmationMenu; }

	EPRFUIState GetMenuState() const { return CurrentState; }
	void SetMenuState(EPRFUIState InUIState);
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

	EPRFUIState CurrentState = EPRFUIState::AnyMenu;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnUIContextChanged, EPRFUIState);
	FOnUIContextChanged OnUIContextChanged;

#pragma endregion

#pragma region Main Menus

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UUserWidget> PressAnyMenu = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> MainMenu = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> NewGameMenu = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> LoadGameMenu = nullptr;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> CreditsMenu = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> QuitMenu = nullptr;
	
#pragma endregion

#pragma region Pause Menus
	
	UPROPERTY()
	TObjectPtr<UUserWidget> PauseMenu = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> OptionsMenu = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> MainMenuConfirmationMenu = nullptr;

	TObjectPtr<UUserWidget> RestartConfirmationMenu = nullptr;

#pragma endregion

#pragma region Option Menus

	UPROPERTY()
	TObjectPtr<UUserWidget> ControlsMenu = nullptr;
#pragma endregion
	
private:
	TArray<TWeakObjectPtr<UUserWidget>> MenuStack;
	TMap<FString, UUserWidget*> MenuClasses;
};
