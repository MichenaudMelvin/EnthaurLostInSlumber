// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultAIController.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
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

bool ADefaultAIController::IsPointReachable(const FVector Point) const
{
	FVector PathStart = GetPawn()->GetActorLocation();
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(GetPawn(), PathStart, Point, NULL);

	if (!NavPath)
	{
		return false;
	}

	return !NavPath->IsPartial();
}

