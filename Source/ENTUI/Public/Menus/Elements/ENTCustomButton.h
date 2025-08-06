// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ENTCustomButton.generated.h"

UCLASS(Abstract)
class ENTUI_API UENTCustomButton : public UUserWidget
{
	GENERATED_BODY()

public:
	TObjectPtr<UButton> GetCustomButton() { return CustomButton; }

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UButton> CustomButton;
};
