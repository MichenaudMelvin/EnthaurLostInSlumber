// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ENTArtificialIntelligenceSubsystem.h"
#include "Interfaces/ENTArtificialIntelligenceInterface.h"

#pragma region Defaults

void UENTArtificialIntelligenceSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UENTArtificialIntelligenceSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (TObjectPtr<UObject> AI : AIList)
	{
		if(AI)
		{
			IENTArtificialIntelligenceInterface::Execute_TickAI(AI, DeltaTime);
		}
	}
}

void UENTArtificialIntelligenceSubsystem::AddAI(UObject* AIObject)
{
	if (!AIObject)
	{
		return;
	}

	if(AIObject->Implements<UENTArtificialIntelligenceInterface>())
	{
		AIList.Add(AIObject);
	}
}

void UENTArtificialIntelligenceSubsystem::RemoveAI(UObject* AIObject)
{
	if (!AIObject)
	{
		return;
	}

	if(AIObject->Implements<UENTArtificialIntelligenceInterface>())
	{
		AIList.Remove(AIObject);
	}
}

#pragma endregion
