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

TArray<AActor*> UENTToolStatics::SortActorsByDistanceToActor(TArray<AActor*> Actors, AActor* Target)
{
	QuickSortByDistance(Actors, 0, Actors.Num() - 1, Target);
	return Actors;
}

void UENTToolStatics::QuickSortByDistance(TArray<AActor*>& InArray, int low, int high, const AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	int i = low;
	int j = high;

	if (!InArray.IsValidIndex(j))
	{
		return;
	}

	// Select a pivot
	double pivot = FVector::DistSquared(InArray[j]->GetActorLocation(), Actor->GetActorLocation());

	while (i <= j)
	{
		if (!InArray.IsValidIndex(i) || !InArray.IsValidIndex(j))
		{
			break;
		}

		/* Find out first pointer, bigger than pivot */
		while (FVector::DistSquared(InArray[i]->GetActorLocation(), Actor->GetActorLocation()) < pivot)
		{
			i++;
		}

		/* Find our second pointer, smaller than pivot */
		while (FVector::DistSquared(InArray[j]->GetActorLocation(), Actor->GetActorLocation()) > pivot)
		{
			j--;
		}
		/* Swap the pointers, calculate new values*/
		if (i <= j)
		{
			InArray.SwapMemory(i++, j--);
		}
	}
	if (j > low)
	{
		QuickSortByDistance(InArray, low, j, Actor);
	}
	if (i < high)
	{
		QuickSortByDistance(InArray, i, high, Actor);
	}
}

