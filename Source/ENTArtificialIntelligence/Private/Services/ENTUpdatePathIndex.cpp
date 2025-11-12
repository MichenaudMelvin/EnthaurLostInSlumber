// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/ENTUpdatePathIndex.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Path/ENTArtificialIntelligencePath.h"

UENTUpdatePathIndex::UENTUpdatePathIndex()
{
	NodeName = "UpdatePathIndex";
	ForceInstancing(true);
	bNotifyBecomeRelevant = true;

	AIPath.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTUpdatePathIndex, AIPath), AENTArtificialIntelligencePath::StaticClass());
	AINextPath.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTUpdatePathIndex, AINextPath),  AENTArtificialIntelligencePath::StaticClass());
	NextPathLocation.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTUpdatePathIndex, NextPathLocation));
	PathIndex.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UENTUpdatePathIndex, PathIndex));
	PathDirection.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UENTUpdatePathIndex, PathDirection));
}

void UENTUpdatePathIndex::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		AIPath.ResolveSelectedKey(*BBAsset);
		AINextPath.ResolveSelectedKey(*BBAsset);
		NextPathLocation.ResolveSelectedKey(*BBAsset);
		PathIndex.ResolveSelectedKey(*BBAsset);
		PathDirection.ResolveSelectedKey(*BBAsset);
	}
}

void UENTUpdatePathIndex::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return;
	}

	UObject* KeyObject = BlackboardComponent->GetValue<UBlackboardKeyType_Object>(AIPath.GetSelectedKeyID());
	if (!KeyObject)
	{
		return;
	}

	AENTArtificialIntelligencePath* PathOBJ = Cast<AENTArtificialIntelligencePath>(KeyObject);
	if (!PathOBJ)
	{
		return;
	}

	int Index = BlackboardComponent->GetValue<UBlackboardKeyType_Int>(PathIndex.GetSelectedKeyID());
	int Direction = BlackboardComponent->GetValue<UBlackboardKeyType_Int>(PathDirection.GetSelectedKeyID());

	if(PathOBJ->IsAtTheEndOfThePath(Index, Direction))
	{
		if (PathOBJ->IsAClosedLoop())
		{
			Index = 0;
		}
		else
		{
			AENTArtificialIntelligencePath* NextPath = PathOBJ->GetNextPath();
			if (NextPath)
			{
				FTransform StartTransform = NextPath->GetStartTransform();
				Index = 1;
				Direction = 1;

				BlackboardComponent->SetValue<UBlackboardKeyType_Vector>(NextPathLocation.GetSelectedKeyID(), StartTransform.GetLocation());
				BlackboardComponent->SetValue<UBlackboardKeyType_Object>(AINextPath.GetSelectedKeyID(), NextPath);
			}
			else if (bCanStopBehaviorIfThePathDoesNotLoop)
			{
				OwnerComp.StopLogic("FinishAIPath");
				return;
			}
			else
			{
				Index -= 1;
				Direction *= -1;
			}

			BlackboardComponent->SetValue<UBlackboardKeyType_Int>(PathDirection.GetSelectedKeyID(), Direction);
		}
	}
	else
	{
		Index += 1 * Direction;
	}

	BlackboardComponent->SetValue<UBlackboardKeyType_Int>(PathIndex.GetSelectedKeyID(), Index);
}

#if WITH_EDITOR
FString UENTUpdatePathIndex::GetStaticDescription() const
{
	FString PathKeyDesc("invalid");
	if (AIPath.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		PathKeyDesc = AIPath.SelectedKeyName.ToString();
	}

	FString PathIndexKeyDesc("invalid");
	if (PathIndex.SelectedKeyType == UBlackboardKeyType_Int::StaticClass())
	{
		PathIndexKeyDesc = PathIndex.SelectedKeyName.ToString();
	}

	FString PathDirectionKeyDesc("invalid");
	if (PathDirection.SelectedKeyType == UBlackboardKeyType_Int::StaticClass())
	{
		PathDirectionKeyDesc = PathDirection.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Update %s (Index: %s, Direction %s)"), *PathKeyDesc, *PathIndexKeyDesc, *PathDirectionKeyDesc );
}
#endif
