// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ENTWidgetBasics.generated.h"

class UENTCustomButton;

UCLASS(Abstract)
class ENTUI_API UENTWidgetBasics : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	virtual void BeginDestroy() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UENTCustomButton> BackButton;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HandleBackAction();
};
