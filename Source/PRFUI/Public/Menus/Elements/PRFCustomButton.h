// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PRFCustomButton.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PRFUI_API UPRFCustomButton : public UUserWidget
{
	GENERATED_BODY()

public:
	TObjectPtr<UButton> GetCustomButton() { return CustomButton; }

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UButton> CustomButton;
};
