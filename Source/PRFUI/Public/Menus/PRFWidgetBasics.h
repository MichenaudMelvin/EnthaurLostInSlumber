// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PRFWidgetBasics.generated.h"

class UButton;

/**
 * 
 */
UCLASS(Abstract)
class PRFUI_API UPRFWidgetBasics : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void BeginDestroy() override;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButton;

	UFUNCTION(BlueprintCallable)
	void HandleBackAction();
};
