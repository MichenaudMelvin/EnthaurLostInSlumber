// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PRFUIManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PRFUI_API UPRFUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// bIsSubMenu is for confirmation boxes that have to not hide the menu just before
	// Can also be used for menus with a fixed part and a inner part that changes (options menu) 
	void OpenMenu(UUserWidget* InMenuClass, bool bIsSubMenu);
	void CloseCurrentMenu();

	UUserWidget* GetPauseMenu() const { return PauseMenu; }

protected:
	void SetUIInputMode() const;
	void SetGameInputMode() const;

	void HandleMenuCollection(UUserWidget* InMenuClass, bool bAddMenu);

	UPROPERTY()
	UUserWidget* PauseMenu = nullptr;

	UPROPERTY()
	UUserWidget* OptionsMenu = nullptr;

private:
	TArray<TWeakObjectPtr<UUserWidget>> MenuStack;
	TMap<FString, TWeakObjectPtr<UUserWidget>> MenuClasses;
};
