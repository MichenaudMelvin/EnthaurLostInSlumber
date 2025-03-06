// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Object.h"
#include "TestViewModel.h"
#include "TestWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRFUI_API UTestWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	UTestViewModel* ViewModel;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdateCounterText();

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CounterText;
};