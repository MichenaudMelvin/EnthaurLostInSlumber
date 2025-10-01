// Fill out your copyright notice in the Description page of Project Settings.


#include "ENTToolStatics.h"
#include "Kismet/KismetMathLibrary.h"

float UENTToolStatics::GetNormalizedFloatRange(float Value, const FFloatRange& FloatRange)
{
	return UKismetMathLibrary::NormalizeToRange(Value, FloatRange.GetLowerBoundValue(), FloatRange.GetUpperBoundValue());
}

float UENTToolStatics::GetAnimLength(UAnimSequenceBase* Anim)
{
	if (!IsValid(Anim))
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Invalid animation"));

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
		return 0;
	}

	return Anim->GetPlayLength() / Anim->RateScale;
}
