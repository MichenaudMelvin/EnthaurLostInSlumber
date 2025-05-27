// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/DefaultSave.h"

#if WITH_EDITOR
void UDefaultSave::PrintVariables() const
{
	UE_LOG(LogBlueprint, Warning, TEXT("Variables of %s%d"), *GetSlotName(), SaveIndex);
}
#endif