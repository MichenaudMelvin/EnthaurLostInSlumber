// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "Menus/ENTWidgetBasics.h"
#include "ENTResetConfirmationMenu.generated.h"

UENUM(BlueprintType)
enum class EENTResetMenuType : uint8
{
	Options,
	Volume,
	Accessibility,
	Brightness,
	Controls
};

UCLASS()
class ENTUI_API UENTResetConfirmationMenu : public UENTWidgetBasics
{
	GENERATED_BODY()

public:
	void SetMenuType(EENTResetMenuType InMenuType);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> ResetButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ResetText;

	UFUNCTION()
	void HandleResetAction();

	EENTResetMenuType MenuType;
};
