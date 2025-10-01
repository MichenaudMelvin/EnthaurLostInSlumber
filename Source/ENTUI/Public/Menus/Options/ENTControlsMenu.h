// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystems.h"
#include "Menus/ENTWidgetBasics.h"
#include "Menus/Elements/ENTInputSlot.h"
#include "ENTControlsMenu.generated.h"

class UVerticalBox;

UCLASS()
class ENTUI_API UENTControlsMenu : public UENTWidgetBasics
{
	GENERATED_BODY()

public:
	void OnKeyButton(UENTInputSlot* InInputSlot);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> GetEnhancedInputLocalPlayerSubsystem();
	void AddInputRows();

	UFUNCTION(BlueprintCallable)
	void RebindKey(const FKey& InKey);
	

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> VBox;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UENTInputSlot> ActiveInputSlot;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UENTInputSlot> InputSlotClass;
};
