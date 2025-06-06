// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "PRFCustomCheckBox.generated.h"

/**
 * 
 */
UCLASS()
class PRFUI_API UPRFCustomCheckBox : public UUserWidget
{
	GENERATED_BODY()

public:
	TObjectPtr<UCheckBox> GetCustomCheckBox() { return CustomCheckBox; }

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UCheckBox> CustomCheckBox;
};
