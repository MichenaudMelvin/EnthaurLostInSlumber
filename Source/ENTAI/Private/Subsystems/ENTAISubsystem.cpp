// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ENTAISubsystem.h"
#include "Interfaces/ENTAIInterface.h"

#pragma region Defaults

void UENTAISubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UENTAISubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (TObjectPtr<UObject> AI : AIList)
	{
		if(AI)
		{
			IENTAIInterface::Execute_TickAI(AI, DeltaTime);
		}
	}
}

void UENTAISubsystem::AddAI(UObject* AIObject)
{
	if (!AIObject)
	{
		return;
	}

	if(AIObject->Implements<UENTAIInterface>())
	{
		AIList.Add(AIObject);
	}
}

void UENTAISubsystem::RemoveAI(UObject* AIObject)
{
	if (!AIObject)
	{
		return;
	}

	if(AIObject->Implements<UENTAIInterface>())
	{
		AIList.Remove(AIObject);
	}
}

#pragma endregion
