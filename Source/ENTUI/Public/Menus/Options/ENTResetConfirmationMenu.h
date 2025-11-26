// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/ENTWidgetBasics.h"
#include "ENTResetConfirmationMenu.generated.h"

/**
 * 
 */
UCLASS()
class ENTUI_API UENTResetConfirmationMenu : public UENTWidgetBasics
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void BeginDestroy() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> ResetButton;

	UFUNCTION()
	void HandleResetAction();
};
