// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorators/ENTCanReachLocation.h"

#include "ENTDefaultAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/KismetSystemLibrary.h"

UENTCanReachLocation::UENTCanReachLocation()
{
	NodeName = "CanReachLocation";
	Location.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTCanReachLocation, Location));
	Location.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTCanReachLocation, Location), AActor::StaticClass());
	bNotifyTick = true;

	ForceInstancing(true);

	ObjectTypes.Add(ObjectTypeQuery1);
	ObjectTypes.Add(ObjectTypeQuery2);
}

void UENTCanReachLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	CheckAbort(OwnerComp, NodeMemory);
}

void UENTCanReachLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		Location.ResolveSelectedKey(*BBAsset);
	}
}

void UENTCanReachLocation::CheckAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
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

bool UENTCanReachLocation::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AENTDefaultAIController* Controller = Cast<AENTDefaultAIController>(OwnerComp.GetAIOwner());
	if (!Controller)
	{
		return false;
	}

	const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CurrentBlackboard)
	{
		return false;
	}

	FVector TargetLocation = FVector::ZeroVector;
	if (Location.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		AActor* TargetActor = Cast<AActor>(CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(Location.SelectedKeyName));
		TargetLocation = TargetActor->GetActorLocation();
	}
	else if (Location.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		TargetLocation = CurrentBlackboard->GetValue<UBlackboardKeyType_Vector>(Location.SelectedKeyName);
	}

	if (bGroundLineTrace)
	{
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(Controller->GetPawn());

		FVector EndLocation = TargetLocation;
		EndLocation.Z -= GroundTraceLength;

		FHitResult HitResult;
		bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, TargetLocation, EndLocation, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, false);

		if (bHit)
		{
			TargetLocation = HitResult.Location;
		}
	}

	return Controller->IsPointReachable(TargetLocation);
}

#if WITH_EDITOR
FString UENTCanReachLocation::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (Location.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() || Location.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		KeyDesc = Location.SelectedKeyName.ToString();
	}

	if (IsInversed())
	{
		return FString::Printf(TEXT("Cannot reach: %s"), *KeyDesc);
	}
	else
	{
		return FString::Printf(TEXT("Can reach: %s"), *KeyDesc);
	}
}
#endif
