// Fill out your copyright notice in the Description page of Project Settings.


#include "ENTDefaultAIController.h"

#include "BrainComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/ENTPawnAIInterface.h"
#include "Subsystems/ENTArtificialIntelligenceSubsystem.h"
#include "Saves/WorldSaves/ENTGameElementData.h"

AENTDefaultAIController::AENTDefaultAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AENTDefaultAIController::BeginPlay()
{
	Super::BeginPlay();

	if (!GetPawn())
	{
		return;
	}

	if (!GetPawn()->Implements<UENTPawnAIInterface>())
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Pawns controlled by %s must implement the IENTPawnAIInterface"), *GetClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
		return;
	}

	if (Cast<IENTPawnAIInterface>(GetPawn())->HasReceivedLoadingRequest())
	{
		LoadControllerData(Cast<IENTPawnAIInterface>(GetPawn())->GetLoadingData());
	}

	if (IENTPawnAIInterface::Execute_DoesAutoStartBehaviorTree(GetPawn()) && !bIsBehaviorTreeRunning)
	{
		RunCurrentBehaviorTree();
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

void AENTDefaultAIController::RunCurrentBehaviorTree()
{
	UBehaviorTree* PawnBehaviorTree = IENTPawnAIInterface::Execute_GetOverridenBehaviorTree(GetPawn());
	UBehaviorTree* TargetBehaviorTree = PawnBehaviorTree ? PawnBehaviorTree : BehaviorTree.Get();

	if (!TargetBehaviorTree)
	{
		return;
	}

	bIsBehaviorTreeRunning = RunBehaviorTree(TargetBehaviorTree);

	if (!GetBlackboardComponent())
	{
		return;
	}

	GetBlackboardComponent()->SetValueAsVector(SpawnLocationKeyName, GetPawn()->GetActorLocation());

	IENTPawnAIInterface::Execute_OnBehaviorTreeStarted(GetPawn());
}

void AENTDefaultAIController::StopBehaviorTree()
{
	GetBrainComponent()->StopLogic("");
	bIsBehaviorTreeRunning = false;
}

#pragma region Saves

void AENTDefaultAIController::SaveControllerData(FENTAIData& AIData)
{
	AIData.bRunningBehaviorTree = bIsBehaviorTreeRunning;
}

void AENTDefaultAIController::LoadControllerData(const FENTAIData& AIData)
{
	if (AIData.bRunningBehaviorTree)
	{
		RunCurrentBehaviorTree();
	}
}

#pragma endregion 