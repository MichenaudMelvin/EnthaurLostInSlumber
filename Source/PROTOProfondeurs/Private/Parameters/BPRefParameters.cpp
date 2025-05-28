// Fill out your copyright notice in the Description page of Project Settings.


#include "Parameters/BPRefParameters.h"

void UBPRefParameters::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	LevelNames.Empty(WorldMap.Num());
	for (auto World : WorldMap)
	{
		LevelNames.Add(World.Key.GetAssetName(), World.Value);
	}
}
