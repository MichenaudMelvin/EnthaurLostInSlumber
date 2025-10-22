// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorators/ENTCheckDistance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UENTCheckDistance::UENTCheckDistance()
{
	NodeName = "CheckDistance";
	Actor.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTCheckDistance, Actor), AActor::StaticClass());
	bNotifyTick = true;
}

void UENTCheckDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	CheckAbort(OwnerComp, NodeMemory);
}

void UENTCheckDistance::CheckAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(FlowAbortMode == EBTFlowAbortMode::None)
	{
		return;
	}

	const bool bIsOnActiveBranch = OwnerComp.IsExecutingBranch(GetMyNode(), GetChildIndex());

	bool bShouldAbort;

	if (bIsOnActiveBranch)
	{
		bShouldAbort = (FlowAbortMode == EBTFlowAbortMode::Self || FlowAbortMode == EBTFlowAbortMode::Both) && !CalculateRawConditionValue(OwnerComp, NodeMemory);
	}
	else
	{
		bShouldAbort = (FlowAbortMode == EBTFlowAbortMode::LowerPriority || FlowAbortMode == EBTFlowAbortMode::Both) && CalculateRawConditionValue(OwnerComp, NodeMemory);
	}

	if(bShouldAbort)
	{
		OwnerComp.RequestExecution(this);
	}
}

bool UENTCheckDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();

	if (!CurrentBlackboard)
	{
		return false;
	}

	const AActor* TargetActor = Cast<AActor>(CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(Actor.SelectedKeyName));

	if (!TargetActor)
	{
		return false;
	}

	const float DistanceToActor = OwnerComp.GetAIOwner()->GetPawn()->GetDistanceTo(TargetActor);

	const float DistanceCheck = Distance.GetValue(OwnerComp);

	bool bResult = false;

	switch (CheckMethod)
	{
		case EENTCheckMethod::Equal:
			bResult = DistanceToActor == DistanceCheck;
			break;
		case EENTCheckMethod::NotEqual:
			bResult = DistanceToActor != DistanceCheck;
			break;
		case EENTCheckMethod::GreaterThanOrEqualTo:
			bResult = DistanceToActor >= DistanceCheck;
			break;
		case EENTCheckMethod::LessOrEqual:
			bResult = DistanceToActor <= DistanceCheck;
			break;
		case EENTCheckMethod::GreaterThan:
			bResult = DistanceToActor > DistanceCheck;
			break;
		case EENTCheckMethod::LessThan:
			bResult = DistanceToActor < DistanceCheck;
			break;
	}

	return bResult;
}
