// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorators/ENTIsLookingAt.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Parasite/ENTParasitePawn.h"

UENTIsLookingAt::UENTIsLookingAt()
{
	NodeName = "IsLookingAt";
	TargetLocation.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTIsLookingAt, TargetLocation), AActor::StaticClass());
	TargetLocation.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTIsLookingAt, TargetLocation));
	bNotifyTick = true;

	ForceInstancing(true);
}

void UENTIsLookingAt::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	CheckAbort(OwnerComp, NodeMemory);
}

void UENTIsLookingAt::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		TargetLocation.ResolveSelectedKey(*BBAsset);
	}
}

void UENTIsLookingAt::CheckAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
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

bool UENTIsLookingAt::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();

	if (!CurrentBlackboard)
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

	FVector PawnLocation = Pawn->GetActorLocation();

	FVector TargetLocationValue;
	if (TargetLocation.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* Object = CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(TargetLocation.SelectedKeyName);

		AActor* Actor = Cast<AActor>(Object);
		if (Actor)
		{
			TargetLocationValue = Actor->GetActorLocation();
		}
	}
	else if (TargetLocation.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		TargetLocationValue = CurrentBlackboard->GetValue<UBlackboardKeyType_Vector>(TargetLocation.SelectedKeyName);
	}

	FVector TargetForward = TargetLocationValue - PawnLocation;
	TargetForward.Normalize();

	FVector CurrentForward;

	AENTParasitePawn* ParasitePawn = Cast<AENTParasitePawn>(Pawn);
	if (ParasitePawn)
	{
		CurrentForward = ParasitePawn->GetParasiteRightVector();
	}
	else
	{
		CurrentForward = Pawn->GetActorForwardVector();
	}

	bool bValue = CurrentForward.Equals(TargetForward, Tolerance);
	return bInverseCheck != bValue;
}

#if WITH_EDITOR
FString UENTIsLookingAt::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (TargetLocation.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() || TargetLocation.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		KeyDesc = TargetLocation.SelectedKeyName.ToString();
	}

	if (bInverseCheck)
	{
		return FString::Printf(TEXT("Is not looking at %s"), *KeyDesc);
	}
	else
	{
		return FString::Printf(TEXT("Is looking at %s"), *KeyDesc);
	}
}
#endif
