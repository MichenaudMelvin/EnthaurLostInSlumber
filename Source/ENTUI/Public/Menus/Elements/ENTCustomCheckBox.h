// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ENTCustomCheckBox.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCheckBoxStateChanged, bool, bIsChecked, bool, bSkipped);

UCLASS()
class ENTUI_API UENTCustomCheckBox : public UUserWidget
{
	GENERATED_BODY()

public:
	TObjectPtr<UButton> GetCustomCheckBox() { return CustomCheckBoxButton; }

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnCheckBoxStateChanged OnCheckBoxStateChanged;

	void SetIsOn(bool bInValue, bool bSkip);

protected:
	virtual void NativeOnInitialized() override;

	virtual void BeginDestroy() override;

	UFUNCTION()
	void HandleButtonClick();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> CustomCheckBoxButton;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool bIsOn = false;
};
