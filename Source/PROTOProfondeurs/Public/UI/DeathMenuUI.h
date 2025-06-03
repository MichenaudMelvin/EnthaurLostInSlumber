// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menus/Elements/PRFCustomButton.h"
#include "DeathMenuUI.generated.h"

/**
 * 
 */
UCLASS()
class PROTOPROFONDEURS_API UDeathMenuUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


	UFUNCTION()
	void RespawnPlayer();
	UFUNCTION()
	void BackToMainMenu();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> RespawnButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> BackMainMenuButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> QuitButton;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> MainMenuLevel;
};
