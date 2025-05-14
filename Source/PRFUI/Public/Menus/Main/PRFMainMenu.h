// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PRFMainMenu.generated.h"

class UPRFUIManager;
class UUIManagerSettings;
class UButton;

/**
 * 
 */
UCLASS()
class PRFUI_API UPRFMainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* NewGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueButton;

	UPROPERTY(meta = (BindWidget))
	UButton* OptionsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CreditsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	virtual void NativeOnInitialized() override;
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

	const UUIManagerSettings* GetUIManagerSettings();
	UPRFUIManager* GetUIManager();

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> TempGameplayLevel;
};
