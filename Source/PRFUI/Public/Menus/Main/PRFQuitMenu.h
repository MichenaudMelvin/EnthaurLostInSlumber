// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/PRFWidgetBasics.h"
#include "PRFQuitMenu.generated.h"

/**
 * 
 */
UCLASS()
class PRFUI_API UPRFQuitMenu : public UPRFWidgetBasics
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UButton* QuitGameButton;

	virtual void NativeOnInitialized() override;
	virtual void BeginDestroy() override;

	UFUNCTION()
	void HandleQuitGameInteraction();
};
