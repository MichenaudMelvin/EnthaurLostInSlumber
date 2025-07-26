// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultAIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Saves/WorldSaves/ENTWorldSave.h"
#include "Subsystems/PRFAISubsystem.h"

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

	UPRFAISubsystem* AISubsystem = GetWorld()->GetSubsystem<UPRFAISubsystem>();
	if(AISubsystem)
	{
		AISubsystem->AddAI(this);
	}
}

void ADefaultAIController::Destroyed()
{
	Super::Destroyed();

	UPRFAISubsystem* AISubsystem = GetWorld()->GetSubsystem<UPRFAISubsystem>();
	if(AISubsystem)
	{
		AISubsystem->RemoveAI(this);
	}
}
void ADefaultAIController::TickAI_Implementation(float DeltaTime)
{
	IAIInterface::TickAI_Implementation(DeltaTime);

	if (GetPawn())
	{
		GetPawn()->Tick(DeltaTime);
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
