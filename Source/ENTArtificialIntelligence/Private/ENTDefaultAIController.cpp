// Fill out your copyright notice in the Description page of Project Settings.


#include "ENTDefaultAIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Subsystems/ENTArtificialIntelligenceSubsystem.h"

AENTDefaultAIController::AENTDefaultAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AENTDefaultAIController::BeginPlay()
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

	UENTArtificialIntelligenceSubsystem* AISubsystem = GetWorld()->GetSubsystem<UENTArtificialIntelligenceSubsystem>();
	if(AISubsystem)
	{
		AISubsystem->AddAI(this);
	}
}

void AENTDefaultAIController::Destroyed()
{
	Super::Destroyed();

	UENTArtificialIntelligenceSubsystem* AISubsystem = GetWorld()->GetSubsystem<UENTArtificialIntelligenceSubsystem>();
	if(AISubsystem)
	{
		AISubsystem->RemoveAI(this);
	}
}
void AENTDefaultAIController::TickAI_Implementation(float DeltaTime)
{
	IENTArtificialIntelligenceInterface::TickAI_Implementation(DeltaTime);

	if (GetPawn())
	{
		GetPawn()->Tick(DeltaTime);
	}
}

bool AENTDefaultAIController::IsPointReachable(const FVector Point) const
{
	FVector PathStart = GetPawn()->GetActorLocation();
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(GetPawn(), PathStart, Point, NULL);

	if (!NavPath)
	{
		return false;
	}

	return !NavPath->IsPartial();
}
