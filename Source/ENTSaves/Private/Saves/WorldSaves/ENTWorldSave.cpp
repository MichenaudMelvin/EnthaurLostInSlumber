// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/WorldSaves/ENTWorldSave.h"

UENTWorldSave::UENTWorldSave()
{
	SlotName = "WorldSave";
}

void UENTWorldSave::ClearMapData()
{
	MuscleData.Empty();
	NerveData.Empty();
	AmberOreData.Empty();
	WeakZoneData.Empty();
	RespawnTreeData.Empty();
	ParasiteData.Empty();
}
