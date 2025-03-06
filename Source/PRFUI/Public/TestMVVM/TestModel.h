// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TestModel.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PRFUI_API UTestModel : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Example")
	int32 Counter = 0;
};