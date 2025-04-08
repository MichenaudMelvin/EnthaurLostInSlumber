// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestView.generated.h"

class UTestModel;
class UTestViewModel;

/**
 * 
 */
UCLASS()
class PRFUI_API UTestView : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* CounterText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* IncreaseButton;

private:
	UPROPERTY()
	TObjectPtr<UTestViewModel> ViewModel;

	UPROPERTY()
	TObjectPtr<UTestModel> Model;

	UFUNCTION()
	void OnIncreaseKeyClicked();
};
