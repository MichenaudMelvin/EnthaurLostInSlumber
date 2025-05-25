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

	UUserWidget* GetPauseMenu() const { return PauseMenu; }
	UUserWidget* GetMainMenu() const { return MainMenu; }
	UUserWidget* GetOptionsMenu() const { return OptionsMenu; }
	UUserWidget* GetCreditsMenu() const { return CreditsMenu; }
	UUserWidget* GetQuitMenu() const { return QuitMenu; }
	UUserWidget* GetNewGameMenu() const { return NewGameMenu; }
	
	EPRFUIState GetMenuState() const { return CurrentState; }
	void SetMenuState(EPRFUIState InUIState);
	void CheckMenuState();

	void SetUIInputMode() const;
	void SetGameInputMode() const;

protected:
	void CreateAllWidgets();
	void CenterCursor() const;

	FDelegateHandle CreateWidgetsDelegate;
	
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
	UUserWidget* PressAnyMenu = nullptr;

	UPROPERTY()
	UUserWidget* MainMenu = nullptr;

	UPROPERTY()
	UUserWidget* NewGameMenu = nullptr;

	UPROPERTY()
	UUserWidget* LoadGameMenu = nullptr;
	
	UPROPERTY()
	UUserWidget* CreditsMenu = nullptr;

	UPROPERTY()
	UUserWidget* QuitMenu = nullptr;
	
#pragma endregion

#pragma region Pause Menus
	
	UPROPERTY()
	UUserWidget* PauseMenu = nullptr;

	UPROPERTY()
	UUserWidget* OptionsMenu = nullptr;

#pragma endregion
	
private:
	TArray<TWeakObjectPtr<UUserWidget>> MenuStack;
	TMap<FString, UUserWidget*> MenuClasses;
};
