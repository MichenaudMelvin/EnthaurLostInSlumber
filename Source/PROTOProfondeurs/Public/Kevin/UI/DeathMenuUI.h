// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathMenuUI.generated.h"

/**
 * 
 */
UCLASS()
class PROTOPROFONDEURS_API UDeathMenuUI : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void RespawnPlayer();
	UFUNCTION()
	void BackToMainMenu();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> RespawnButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackMainMenuButton;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UWorld> MainMenuLevel;
};
