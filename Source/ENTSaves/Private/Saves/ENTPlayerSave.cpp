// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/ENTPlayerSave.h"

UENTPlayerSave::UENTPlayerSave()
{
	SlotName = "PlayerSave";

	AmberInventory.Add(0, 0);
	AmberInventory.Add(1, 0);
}
