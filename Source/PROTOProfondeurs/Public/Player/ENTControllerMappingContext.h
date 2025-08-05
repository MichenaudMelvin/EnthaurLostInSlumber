// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ENTControllerMappingContext.generated.h"

class UInputMappingContext;

// This class does not need to be modified.
UINTERFACE()
class PROTOPROFONDEURS_API UENTControllerMappingContext : public UInterface
{
	GENERATED_BODY()
};

class PROTOPROFONDEURS_API IENTControllerMappingContext
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual TObjectPtr<UInputMappingContext> GetDefaultMappingContext() const = 0;

	virtual TObjectPtr<UInputMappingContext> GetUIMappingContext() const = 0;

	virtual TObjectPtr<UInputMappingContext> GetAnyKeyMappingContext() const = 0;
};
