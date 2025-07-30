// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/ENTClearBlackboardVariable.h"
#include "BehaviorTree/BlackboardComponent.h"

UENTClearBlackboardVariable::UENTClearBlackboardVariable()
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

void UENTClearBlackboardVariable::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		BlackboardKey.ResolveSelectedKey(*BBAsset);
	}
}

void UENTClearBlackboardVariable::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (bExecuteAtStart)
	{
		ClearValue(OwnerComp);
	}
}

void UENTClearBlackboardVariable::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	if (!bExecuteAtStart)
	{
		ClearValue(OwnerComp);
	}
}

void UENTClearBlackboardVariable::ClearValue(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();

	if (!CurrentBlackboard)
	{
		return;
	}

	CurrentBlackboard->ClearValue(BlackboardKey.GetSelectedKeyID());
}
