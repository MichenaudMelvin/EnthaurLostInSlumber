// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ENTAnyKeyController.h"

void AENTAnyKeyController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnAnyKeyPressed.Clear();
}
