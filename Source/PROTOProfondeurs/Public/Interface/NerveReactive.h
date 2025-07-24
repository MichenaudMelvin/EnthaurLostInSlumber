// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NerveReactive.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UNerveReactive : public UInterface
{
	GENERATED_BODY()
};

class PROTOPROFONDEURS_API INerveReactive
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Trigger();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetLock(bool bState);
};
