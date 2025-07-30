// Fill out your copyright notice in the Description page of Project Settings.


#include "ENTDefaultAIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Subsystems/ENTAISubsystem.h"

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

	UENTAISubsystem* AISubsystem = GetWorld()->GetSubsystem<UENTAISubsystem>();
	if(AISubsystem)
	{
		AISubsystem->AddAI(this);
	}
}

void AENTDefaultAIController::Destroyed()
{
	Super::Destroyed();

	UENTAISubsystem* AISubsystem = GetWorld()->GetSubsystem<UENTAISubsystem>();
	if(AISubsystem)
	{
		AISubsystem->RemoveAI(this);
	}
}
void AENTDefaultAIController::TickAI_Implementation(float DeltaTime)
{
	IENTAIInterface::TickAI_Implementation(DeltaTime);

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
