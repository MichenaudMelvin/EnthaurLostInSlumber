// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TestModel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCounterChanged, int32, NewCounterValue);

/**
 * 
 */
UCLASS(Blueprintable)
class PRFUI_API UTestModel : public UObject
{
	GENERATED_BODY()

public:
	UTestModel();

	UFUNCTION(BlueprintCallable)
	void IncreaseCounter(int32 Amount);

	UFUNCTION(BlueprintPure)
	int32 GetCounter() const;

	UPROPERTY(BlueprintAssignable)
	FOnCounterChanged OnCounterChanged;

private:
	int32 Counter;
};
