// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTOverrideParasiteVelocity.h"

#include "AIController.h"
#include "Parasite/ENTParasitePawn.h"

UENTOverrideParasiteVelocity::UENTOverrideParasiteVelocity()
{
	NodeName = "OverrideParasiteVelocity";

	ForceInstancing(true);
}

EBTNodeResult::Type UENTOverrideParasiteVelocity::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	AENTParasitePawn* ParasitePawn = Cast<AENTParasitePawn>(Pawn);
	if (!ParasitePawn)
	{
		return EBTNodeResult::Failed;
	}

	ParasitePawn->OverrideVelocity(OverridenVelocity);

	return EBTNodeResult::Succeeded;
}

#if WITH_EDITOR
FString UENTOverrideParasiteVelocity::GetStaticDescription() const
{
	if (OverridenVelocity == FVector::ZeroVector)
	{
		return FString::Printf(TEXT("Return to normal velocity"));
	}
	else
	{
		return FString::Printf(TEXT("Override velocity: %s"), *OverridenVelocity.ToString());
	}
}
#endif
