// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/ENTUpdatePathIndex.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Components/SplineComponent.h"
#include "Path/ENTArtificialIntelligencePath.h"

UENTUpdatePathIndex::UENTUpdatePathIndex()
{
	NodeName = "UpdatePathIndex";
	ForceInstancing(true);
	bNotifyBecomeRelevant = true;

	AIPath.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTUpdatePathIndex, AIPath), AENTArtificialIntelligencePath::StaticClass());
	PathIndex.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UENTUpdatePathIndex, PathIndex));
	PathDirection.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UENTUpdatePathIndex, PathDirection));
}

void UENTUpdatePathIndex::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return;
	}

	UObject* KeyObject = BlackboardComponent->GetValueAsObject("AIPath");
	// UObject* KeyObject = BlackboardComponent->GetValue<UBlackboardKeyType_Object>(Path.GetSelectedKeyID());
	if (!KeyObject)
	{
		return;
	}

	AENTArtificialIntelligencePath* PathOBJ = Cast<AENTArtificialIntelligencePath>(KeyObject);
	if (!PathOBJ)
	{
		return;
	}

	int Index = BlackboardComponent->GetValueAsInt("PathIndex");
	int Direction = BlackboardComponent->GetValueAsInt("PathDirection");
	// int Index = BlackboardComponent->GetValue<UBlackboardKeyType_Int>(PathIndex.GetSelectedKeyID());
	// int Direction = BlackboardComponent->GetValue<UBlackboardKeyType_Int>(PathDirection.GetSelectedKeyID());

	if(Index == PathOBJ->GetSpline()->GetNumberOfSplinePoints() - 1)
	{
		if (PathOBJ->GetSpline()->IsClosedLoop())
		{
			Index = 0;
		}
		else
		{
			Index -= 1;
			Direction *= -1;

			BlackboardComponent->SetValueAsInt("PathDirection", Direction);
			// BlackboardComponent->SetValue<UBlackboardKeyType_Int>(PathDirection.GetSelectedKeyID(), Direction);
		}
	}
	else
	{
		Index += 1 * Direction;
	}

	BlackboardComponent->SetValueAsInt("PathIndex", Index);
	// BlackboardComponent->SetValue<UBlackboardKeyType_Int>(AIPath.GetSelectedKeyID(), Index);
}
