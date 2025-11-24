// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/ENTChangeSpeed.h"

#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "GameFramework/FloatingPawnMovement.h"

UENTChangeSpeed::UENTChangeSpeed()
{
	NodeName = "ChangeSpeed";
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;

	ForceInstancing(true);
}

void UENTChangeSpeed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (bExecuteAtStart)
	{
		ChangeSpeed(OwnerComp);
	}
}

void UENTChangeSpeed::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	if (!bExecuteAtStart)
	{
		ChangeSpeed(OwnerComp);
	}
}

#if WITH_EDITOR
FString UENTChangeSpeed::GetStaticDescription() const
{
	return FString::Printf(TEXT("Change speed to %s"), *TargetSpeedValue.ToString());
}
#endif

void UENTChangeSpeed::ChangeSpeed(UBehaviorTreeComponent& OwnerComp)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return;
	}

	UFloatingPawnMovement* MovementComp = Pawn->GetComponentByClass<UFloatingPawnMovement>();
	if (!MovementComp)
	{
		return;
	}

	UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CurrentBlackboard)
	{
		return;
	}

	MovementComp->MaxSpeed = CurrentBlackboard->GetValue<UBlackboardKeyType_Float>(TargetSpeedValue.GetKey());
}
