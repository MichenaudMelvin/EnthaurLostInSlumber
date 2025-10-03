// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ENTAnyKeyController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnyKeyPressed, const FKey&, KeyPressed);

UCLASS(Abstract)
class ENTCORE_API AENTAnyKeyController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/**
	 * @brief Must call this by a blueprint key event
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAnyKeyPressed OnAnyKeyPressed;
};