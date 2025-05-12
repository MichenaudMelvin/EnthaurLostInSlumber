// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/WorldSaves/WorldSave.h"

UWorldSave::UWorldSave()
{
	SlotName = "WorldSave";
}

void UWorldSave::ClearMapData()
{
	MuscleData.Empty();
	NerveData.Empty();
	AmberOreData.Empty();
	WeakZoneData.Empty();
	RespawnTreeData.Empty();
}
