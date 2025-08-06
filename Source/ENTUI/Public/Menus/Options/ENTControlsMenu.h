// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Menus/ENTWidgetBasics.h"
#include "ENTControlsMenu.generated.h"

UCLASS()
class ENTUI_API UENTControlsMenu : public UENTWidgetBasics
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
};
