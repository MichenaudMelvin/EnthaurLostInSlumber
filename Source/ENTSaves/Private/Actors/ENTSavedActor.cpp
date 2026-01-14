// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ENTSavedActor.h"
#include "Saves/WorldSaves/ENTGameElementData.h"

void AENTSavedActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!SaveID.IsValid())
	{
		SaveID = FGuid::NewGuid();

#if WITH_EDITORONLY_DATA
		const FString Message = FString::Printf(TEXT("%s has generated a FGuid, make sure to save the asset (or the current level) to ensure the asset will be correctly saved"), *GetActorLabel());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
	}
}

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
