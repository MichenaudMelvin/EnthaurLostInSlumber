// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/ENTSetRandomLocation.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTService.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UENTSetRandomLocation::UENTSetRandomLocation()
{
	bNotifyBecomeRelevant = true;
	bCreateNodeInstance = true;

	RandomLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTSetRandomLocation, RandomLocationKey));
	Origin.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTSetRandomLocation, Origin));
}

void UENTSetRandomLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		RandomLocationKey.ResolveSelectedKey(*BBAsset);
		Origin.ResolveSelectedKey(*BBAsset);
	}
}

void UENTSetRandomLocation::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (!ActorOwner)
	{
		return;
	}

	FNavLocation RandomLocationResult;
	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(ActorOwner);
	if (!NavData)
	{
		NavData = NavigationSystem->MainNavData;
	}

	UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();

	if (!CurrentBlackboard)
	{
		return;
	}

	FSharedConstNavQueryFilter NavQueryFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, ActorOwner, NavFilter);

	FVector TargetLocation = bUsePawnLocation ? ActorOwner->GetActorLocation() : CurrentBlackboard->GetValue<UBlackboardKeyType_Vector>(Origin.GetSelectedKeyID());

	bool bSuccess = NavigationSystem->GetRandomPointInNavigableRadius(TargetLocation, Radius, RandomLocationResult, NavData, NavQueryFilter);

	if (!bSuccess)
	{
		return;
	}

	CurrentBlackboard->SetValue<UBlackboardKeyType_Vector>(RandomLocationKey.GetSelectedKeyID(), RandomLocationResult.Location);
}

void UENTSetRandomLocation::SetOwner(AActor* InActorOwner)
{
	Super::SetOwner(InActorOwner);

	ActorOwner = InActorOwner;
}
