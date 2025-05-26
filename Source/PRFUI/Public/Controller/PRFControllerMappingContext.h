// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PRFControllerMappingContext.generated.h"

class UInputMappingContext;

// This class does not need to be modified.
UINTERFACE()
class UPRFControllerMappingContext : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PRFUI_API IPRFControllerMappingContext
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual TObjectPtr<UInputMappingContext> GetUIMappingContext() const = 0;
	virtual TObjectPtr<UInputMappingContext> GetAnyKeyMappingContext() const = 0;
	virtual TObjectPtr<UInputMappingContext> GetDefaultMappingContext() const = 0;
};
