// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ENTLevelEntering.generated.h"

class UTextBlock;

UCLASS()
class ENTUI_API UENTLevelEntering : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Text", meta = (BindWidget))
	TObjectPtr<UTextBlock> AreaName;

	UPROPERTY(BlueprintReadWrite, Category = "Text", meta = (BindWidget))
	TObjectPtr<UTextBlock> RegionName;
};
