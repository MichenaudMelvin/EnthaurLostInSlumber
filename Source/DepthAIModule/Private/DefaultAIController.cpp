// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

ADefaultAIController::ADefaultAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADefaultAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}

	if (GetBlackboardComponent() && GetPawn())
	{
		GetBlackboardComponent()->SetValueAsVector(SpawnLocationKeyName, GetPawn()->GetActorLocation());
	}
}

