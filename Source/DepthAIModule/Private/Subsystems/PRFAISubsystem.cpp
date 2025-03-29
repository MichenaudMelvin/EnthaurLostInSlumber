// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/PRFAISubsystem.h"

#include "Interfaces/AIInterface.h"

#pragma region Defaults

void UPRFAISubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UPRFAISubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (TObjectPtr<UObject> AI : AIList)
	{
		if(AI)
		{
			IAIInterface::Execute_TickAI(AI, DeltaTime);
		}
	}
}

void UPRFAISubsystem::AddAI(UObject* AIObject)
{
	if (!AIObject)
	{
		return;
	}

	if(AIObject->Implements<UAIInterface>())
	{
		AIList.Add(AIObject);
	}
}

void UPRFAISubsystem::RemoveAI(UObject* AIObject)
{
	if (!AIObject)
	{
		return;
	}

	if(AIObject->Implements<UAIInterface>())
	{
		AIList.Remove(AIObject);
	}
}

#pragma endregion
