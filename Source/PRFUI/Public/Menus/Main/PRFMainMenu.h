// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/PRFWidgetBasics.h"
#include "Menus/Elements/PRFCustomButton.h"
#include "PRFMainMenu.generated.h"

class UPRFUIManager;
class UUIManagerSettings;
class UButton;

/**
 * 
 */
UCLASS()
class PRFUI_API UPRFMainMenu : public UPRFWidgetBasics
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> NewGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> ContinueButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> OptionsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> CreditsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> QuitButton;

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	virtual void BeginDestroy() override;

	UFUNCTION()
	void HandleNewGameMenu();

	UFUNCTION()
	void HandleContinueInteraction();

	UFUNCTION()
	void HandleOptionsMenu();

	UFUNCTION()
	void HandleCreditsMenu();

	UFUNCTION()
	void HandleQuitMenu();

	const UUIManagerSettings* GetUIManagerSettings() const;
	UPRFUIManager* GetUIManager() const;
};
