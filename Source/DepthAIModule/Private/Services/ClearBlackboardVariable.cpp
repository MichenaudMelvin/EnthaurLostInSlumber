// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/ClearBlackboardVariable.h"
#include "BehaviorTree/BlackboardComponent.h"

UClearBlackboardVariable::UClearBlackboardVariable()
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

void UClearBlackboardVariable::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		BlackboardKey.ResolveSelectedKey(*BBAsset);
	}
}

void UClearBlackboardVariable::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (bExecuteAtStart)
	{
		ClearValue(OwnerComp);
	}
}

void UClearBlackboardVariable::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	if (!bExecuteAtStart)
	{
		ClearValue(OwnerComp);
	}
}

void UClearBlackboardVariable::ClearValue(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();

	if (!CurrentBlackboard)
	{
		return;
	}

	CurrentBlackboard->ClearValue(BlackboardKey.GetSelectedKeyID());
}
