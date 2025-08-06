// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/ENTWidgetBasics.h"
#include "ENTNewGameMenu.generated.h"

UCLASS()
class ENTUI_API UENTNewGameMenu : public UENTWidgetBasics
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> ContinueButton;

	virtual void NativeOnInitialized() override;

	virtual void BeginDestroy() override;

	UFUNCTION()
	void HandleNewGameInteraction();
};
