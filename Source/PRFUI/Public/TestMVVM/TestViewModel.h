// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestModel.h"
#include "UObject/Object.h"
#include "TestViewModel.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCounterUpdated);

UCLASS(Blueprintable)
class PRFUI_API UTestViewModel : public UObject
{
	GENERATED_BODY()

private:
	int32 Counter = 0;

public:
	UPROPERTY(BlueprintAssignable)
	FOnCounterUpdated OnCounterUpdated;

	UFUNCTION(BlueprintCallable)
	void IncreaseCounter()
	{
		Counter++;
		OnCounterUpdated.Broadcast();
	}

	UFUNCTION(BlueprintPure)
	FText GetCounterText() const
	{
		return FText::FromString(FString::Printf(TEXT("%d"), Counter));
	}
};