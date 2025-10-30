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
	/**
	 * @brief Returns Value normalized to the given FFloatRange. (e.g. 20 normalized to the range 10->50 would result in 0.25)
	 * @param Value 
	 * @param FloatRange 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math")
	static float GetNormalizedFloatRange(float Value, const FFloatRange& FloatRange);

	/**
	 * @brief Return the anim length (duration in seconds) with the rate scale
	 * @param Anim 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	static float GetAnimLength(UAnimSequenceBase* Anim);
};
