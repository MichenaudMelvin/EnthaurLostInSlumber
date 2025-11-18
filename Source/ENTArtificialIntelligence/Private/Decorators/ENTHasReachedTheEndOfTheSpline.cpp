// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorators/ENTHasReachedTheEndOfTheSpline.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Parasite/ENTParasitePawn.h"
#include "Path/ENTArtificialIntelligencePath.h"

UENTHasReachedTheEndOfTheSpline::UENTHasReachedTheEndOfTheSpline()
{
	NodeName = "HasReachedTheEndOfTheSpline";
	AIPath.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTHasReachedTheEndOfTheSpline, AIPath), AENTArtificialIntelligencePath::StaticClass());
	PathIndex.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UENTHasReachedTheEndOfTheSpline, PathIndex));
	PathDirection.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UENTHasReachedTheEndOfTheSpline, PathDirection));
	bNotifyTick = true;
}

void UENTHasReachedTheEndOfTheSpline::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	CheckAbort(OwnerComp, NodeMemory);
}

void UENTHasReachedTheEndOfTheSpline::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		AIPath.ResolveSelectedKey(*BBAsset);
		PathIndex.ResolveSelectedKey(*BBAsset);
		PathDirection.ResolveSelectedKey(*BBAsset);
	}
}

void UENTHasReachedTheEndOfTheSpline::CheckAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
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

bool UENTHasReachedTheEndOfTheSpline::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return false;
	}

	UObject* KeyObject = BlackboardComponent->GetValue<UBlackboardKeyType_Object>(AIPath.GetSelectedKeyID());
	if (!KeyObject)
	{
		return false;
	}

	AENTArtificialIntelligencePath* PathOBJ = Cast<AENTArtificialIntelligencePath>(KeyObject);
	if (!PathOBJ)
	{
		return false;
	}

	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return false;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return false;
	}

	float PawnHeight = 0.0f;

	AENTParasitePawn* Parasite = Cast<AENTParasitePawn>(Pawn);
	if (Parasite)
	{
		PawnHeight = Parasite->GetHitBoxHeight();
	}

	int Direction = BlackboardComponent->GetValue<UBlackboardKeyType_Int>(PathDirection.GetSelectedKeyID());

	return !PathOBJ->IsAtTheEndOfThePath(Pawn->GetActorLocation(), PawnHeight, Direction, Tolerance);
}

#if WITH_EDITOR
FString UENTHasReachedTheEndOfTheSpline::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (AIPath.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		KeyDesc = AIPath.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Has reached the end of %s"), *KeyDesc);
}
#endif
