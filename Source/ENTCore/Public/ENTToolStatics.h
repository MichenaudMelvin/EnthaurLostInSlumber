// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ENTToolStatics.generated.h"

UCLASS()
class ENTCORE_API UENTToolStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	static float GetAnimLength(UAnimSequenceBase* Anim);
};
