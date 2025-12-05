// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/ENTComputeClosestReachablePoint.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

#if WITH_EDITORONLY_DATA
#include "Kismet/KismetSystemLibrary.h"
#endif

UENTComputeClosestReachablePoint::UENTComputeClosestReachablePoint()
{
	NodeName = "ComputeClosestReachablePoint";
	ForceInstancing(true);
	bNotifyBecomeRelevant = true;

	Point.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTComputeClosestReachablePoint, Point));
	Point.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTComputeClosestReachablePoint, Point), AActor::StaticClass());

	ClosestPoint.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTComputeClosestReachablePoint, ClosestPoint));
}

void UENTComputeClosestReachablePoint::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		Point.ResolveSelectedKey(*BBAsset);
		ClosestPoint.ResolveSelectedKey(*BBAsset);
	}
}

void UENTComputeClosestReachablePoint::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CurrentBlackboard)
	{
		return;
	}

	FVector TargetPoint = FVector::ZeroVector;
	if (Point.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		AActor* TargetActor = Cast<AActor>(CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(Point.GetSelectedKeyID()));

		if (!TargetActor)
		{
			return;
		}

		TargetPoint = TargetActor->GetActorLocation();
	}
	else if (Point.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		TargetPoint = CurrentBlackboard->GetValue<UBlackboardKeyType_Vector>(Point.GetSelectedKeyID());
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(this);
	if (!NavSystem)
	{
		return;
	}

	FNavLocation Result;
	if (!NavSystem->ProjectPointToNavigation(TargetPoint, Result, FVector(QueryExtent)))
	{
		return;
	}

#if WITH_EDITORONLY_DATA
	if (bDebugService)
	{
		UKismetSystemLibrary::DrawDebugPoint(this, Result.Location, 25.0f, FLinearColor::Red, 10.0f);
	}
#endif

	CurrentBlackboard->SetValueAsVector(ClosestPoint.SelectedKeyName, Result.Location);
}

#if WITH_EDITOR
FString UENTComputeClosestReachablePoint::GetStaticDescription() const
{
	FString PointKeyDesc("invalid");
	if (Point.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() || Point.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		PointKeyDesc = Point.SelectedKeyName.ToString();
	}

	FString ClosestPointKeyDesc("invalid");
	if (ClosestPoint.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		ClosestPointKeyDesc = ClosestPoint.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Compute %s to become the closest point (%s)"), *PointKeyDesc, *ClosestPointKeyDesc);
}
#endif
