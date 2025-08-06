// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/ENTWidgetBasics.h"
#include "ENTRestartConfirmationMenu.generated.h"

UCLASS()
class ENTUI_API UENTRestartConfirmationMenu : public UENTWidgetBasics
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void BeginDestroy() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> RestartButton;

	UFUNCTION()
	void HandleRestartAction();
};
