// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menus/ENTWidgetBasics.h"
#include "ENTPressAny.generated.h"

UCLASS()
class ENTUI_API UENTPressAny : public UENTWidgetBasics
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
};
