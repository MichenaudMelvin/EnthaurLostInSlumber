// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ENTInputSlot.generated.h"

class UENTCustomButton;
class UENTControlsMenu;
class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class ENTUI_API UENTInputSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetKeyMappingName(const FName& InKeyMappingName);
	void SetKeyName(const FText& InKeyName);
	void SetButtonKeyName(const FText& InButtonKeyName);
	void SetControlsMenu(UENTControlsMenu* InControlsMenu);
	FName GetMappingName();
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void BeginDestroy() override;

	UFUNCTION()
	void OnKeyButtonPressed();
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> KeyName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> Button;

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn), EditInstanceOnly)
	FName KeyMappingName;

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn), EditInstanceOnly)
	TObjectPtr<UENTControlsMenu> ControlsMenu;
};
