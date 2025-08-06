// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/ENTWidgetBasics.h"
#include "Menus/Elements/ENTCustomButton.h"
#include "ENTMainMenu.generated.h"

class UENTMenuManager;
class UENTUIConfig;

UCLASS()
class ENTUI_API UENTMainMenu : public UENTWidgetBasics
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> NewGameButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> ContinueButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> OptionsButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> CreditsButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> QuitButton;

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

	const UENTUIConfig* GetUIConfig() const;

	UENTMenuManager* GetMenuManager() const;
};
