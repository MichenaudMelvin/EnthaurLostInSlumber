// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ENTSavedActor.h"
#include "Saves/WorldSaves/ENTGameElementData.h"

#if WITH_EDITORONLY_DATA
void AENTSavedActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsValidChecked(this))
	{
		return;
	}

	UWorld* World = GetWorld();

	const bool bPlacedInWorld = (World->bStartup);

	if (!SaveID.IsValid() && bPlacedInWorld)
	{
		const FString Message = FString::Printf(TEXT("Generate the SaveID of %s and make sure to save the asset (or the current level) to ensure the asset will be correctly saved"), *GetActorLabel());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
	}
}
#endif

FENTGameElementData& AENTSavedActor::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	return EmptyData;
}

void AENTSavedActor::LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave)
{
	bIsLoading = true;
}

void AENTSavedActor::FinishLoading(UENTWorldSave* LoadedWorldSave)
{
	bIsLoading = false;
	bIsLoaded = true;
}
