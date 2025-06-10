// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/PRFWidgetBasics.h"
#include "PRFRestartConfirmationMenu.generated.h"

/**
 * 
 */
UCLASS()
class PRFUI_API UPRFRestartConfirmationMenu : public UPRFWidgetBasics
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UPRFCustomButton> RestartButton;
	
	UFUNCTION()
	void HandleRestartAction();
};
