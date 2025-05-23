// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUI.generated.h"

/**
 * 
 */
UCLASS()
class PROTOPROFONDEURS_API UMainMenuUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> LevelToLoad;

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
};
