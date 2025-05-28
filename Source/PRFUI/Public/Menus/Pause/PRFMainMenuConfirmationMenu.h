// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/PRFWidgetBasics.h"
#include "PRFMainMenuConfirmationMenu.generated.h"

/**
 * 
 */
UCLASS()
class PRFUI_API UPRFMainMenuConfirmationMenu : public UPRFWidgetBasics
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> MainMenuButton;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> MainMenuLevel;

	UFUNCTION()
	void HandleMainMenuAction();
};
