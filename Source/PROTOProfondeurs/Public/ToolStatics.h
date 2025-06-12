// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ToolStatics.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UToolStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	static float GetAnimLength(UAnimSequenceBase* Anim);
};
