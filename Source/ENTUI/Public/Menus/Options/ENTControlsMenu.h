// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/ENTWidgetBasics.h"
#include "ENTControlsMenu.generated.h"

class UENTInputSlot;
class UVerticalBox;

UCLASS()
class ENTUI_API UENTControlsMenu : public UENTWidgetBasics
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void AddInputRows();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> VBox;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UENTInputSlot> InputSlotClass;
};
