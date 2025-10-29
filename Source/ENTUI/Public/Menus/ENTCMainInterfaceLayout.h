// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ENTCommonWidgetBase.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "ENTCMainInterfaceLayout.generated.h"

/**
 * 
 */
UCLASS()
class ENTUI_API UENTCMainInterfaceLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	TObjectPtr<UCommonActivatableWidgetContainerBase> GetWidgetStack() const { return WidgetStack; }
	
	UFUNCTION(BlueprintCallable)
	void AddWidgetToStack(UCommonActivatableWidget* InWidget) const;

	UFUNCTION(BlueprintCallable)
	void RemoveWidgetFromStack(UCommonActivatableWidget* InWidget) const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetContainerBase> WidgetStack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Menu References")
	TSubclassOf<UENTCommonWidgetBase> MainMenu = nullptr;
};
