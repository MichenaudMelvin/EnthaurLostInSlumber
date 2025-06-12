// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PRFCustomCheckBox.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCheckBoxStateChanged, bool, bIsChecked, bool, bSkipped);
/**
 * 
 */
UCLASS()
class PRFUI_API UPRFCustomCheckBox : public UUserWidget
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
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UButton> CustomCheckBoxButton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsOn = false;
};
