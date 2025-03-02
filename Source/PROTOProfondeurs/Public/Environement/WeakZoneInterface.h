// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeakZoneInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UWeakZoneInterface : public UInterface
{
	GENERATED_BODY()
};

class PROTOPROFONDEURS_API IWeakZoneInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category = "GroundAction")
	void OnEnterWeakZone(bool bIsZoneActive);

	UFUNCTION(BlueprintNativeEvent, Category = "GroundAction")
	void OnExitWeakZone();
};
