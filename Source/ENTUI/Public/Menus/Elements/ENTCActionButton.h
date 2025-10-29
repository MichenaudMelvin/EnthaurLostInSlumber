// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonActionWidget.h"
#include "CommonTextBlock.h"
#include "ENTCActionButton.generated.h"

/**
 * 
 */
UCLASS()
class ENTUI_API UENTCActionButton : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual bool NativeOnHandleBackAction() override;

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UCommonActionWidget> ActionWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UCommonTextBlock> TextBlock;
};
