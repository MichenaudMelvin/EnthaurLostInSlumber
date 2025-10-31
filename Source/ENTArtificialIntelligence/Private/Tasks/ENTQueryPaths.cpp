// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTQueryPaths.h"

#include "AIController.h"
#include "ENTToolStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/GameplayStatics.h"
#include "Path/ENTArtificialIntelligencePath.h"
#include "Path/ENTNavigationArea.h"

UENTQueryPaths::UENTQueryPaths()
{
	NodeName = "QueryPaths";

	AIPathKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTQueryPaths, AIPathKey), AENTArtificialIntelligencePath::StaticClass());
	NavAreaKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTQueryPaths, NavAreaKey), AENTNavigationArea::StaticClass());
}

void UENTQueryPaths::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		AIPathKey.ResolveSelectedKey(*BBAsset);
		NavAreaKey.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UENTQueryPaths::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CurrentBlackboard)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	TArray<AActor*> Paths;
	TArray<AActor*> NavArea;

	UGameplayStatics::GetAllActorsOfClass(Pawn, AENTArtificialIntelligencePath::StaticClass(), Paths);
	UGameplayStatics::GetAllActorsOfClass(Pawn, AENTNavigationArea::StaticClass(), NavArea);

	if (bSearchForPaths)
	{
		if (Paths.IsEmpty())
		{
			return EBTNodeResult::Failed;
		}

		if (bIgnoreCurrentValue)
		{
			UObject* Path = CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(AIPathKey.GetSelectedKeyID());
			if (Path)
			{
				Paths.Remove(Cast<AENTArtificialIntelligencePath>(Path));
			}
		}

		Paths = UENTToolStatics::SortActorsByDistanceToActor(Paths, Pawn);
	}

	if (bSearchForNavArea)
	{
		if (NavArea.IsEmpty())
		{
			return EBTNodeResult::Failed;
		}

		if (bIgnoreCurrentValue)
		{
			UObject* Area = CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(NavAreaKey.GetSelectedKeyID());
			if (Area)
			{
				NavArea.Remove(Cast<AENTArtificialIntelligencePath>(Area));
			}
		}

		NavArea = UENTToolStatics::SortActorsByDistanceToActor(NavArea, Pawn);
	}

#if WITH_EDITORONLY_DATA
	AActor* ClosestActor = nullptr;
#endif

	if (bSearchForPaths && bSearchForNavArea)
	{
		float DistanceToClosestPath = Paths[0]->GetDistanceTo(Pawn);
		float DistanceToClosestNavArea = NavArea[0]->GetDistanceTo(Pawn);

		if (DistanceToClosestPath < DistanceToClosestNavArea)
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Object>(AIPathKey.GetSelectedKeyID(), Paths[0]);
			CurrentBlackboard->SetValue<UBlackboardKeyType_Object>(NavAreaKey.GetSelectedKeyID(), nullptr);

#if WITH_EDITORONLY_DATA
			ClosestActor = Paths[0];
#endif
		}
		else if (DistanceToClosestPath > DistanceToClosestNavArea)
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Object>(NavAreaKey.GetSelectedKeyID(), NavArea[0]);
			CurrentBlackboard->SetValue<UBlackboardKeyType_Object>(AIPathKey.GetSelectedKeyID(), nullptr);

#if WITH_EDITORONLY_DATA
			ClosestActor = NavArea[0];
#endif
		}
		else if (DistanceToClosestPath == DistanceToClosestNavArea)
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Object>(AIPathKey.GetSelectedKeyID(), Paths[0]);
			CurrentBlackboard->SetValue<UBlackboardKeyType_Object>(NavAreaKey.GetSelectedKeyID(), NavArea[0]);
		}
	}
	else if (bSearchForPaths && !bSearchForNavArea)
	{
		CurrentBlackboard->SetValue<UBlackboardKeyType_Object>(AIPathKey.GetSelectedKeyID(), Paths[0]);
		CurrentBlackboard->SetValue<UBlackboardKeyType_Object>(NavAreaKey.GetSelectedKeyID(), nullptr);

#if WITH_EDITORONLY_DATA
		ClosestActor = Paths[0];
#endif
	}
	else if (!bSearchForPaths && bSearchForNavArea)
	{
		CurrentBlackboard->SetValue<UBlackboardKeyType_Object>(NavAreaKey.GetSelectedKeyID(), NavArea[0]);
		CurrentBlackboard->SetValue<UBlackboardKeyType_Object>(AIPathKey.GetSelectedKeyID(), nullptr);

#if WITH_EDITORONLY_DATA
		ClosestActor = NavArea[0];
#endif
	}
	else
	{
		CurrentBlackboard->SetValue<UBlackboardKeyType_Object>(AIPathKey.GetSelectedKeyID(), nullptr);
		CurrentBlackboard->SetValue<UBlackboardKeyType_Object>(NavAreaKey.GetSelectedKeyID(), nullptr);
	}

#if WITH_EDITORONLY_DATA
	if(bDebugTask)
	{
		FVector TextLocation;
		FString Text;
		if (ClosestActor)
		{
			TextLocation = ClosestActor->GetActorLocation();
			Text = ClosestActor->GetActorLabel();
			Text += " is the closest actor";
		}
		else
		{
			TextLocation = Pawn->GetActorLocation();
			Text = "No actors found";
		}

		UKismetSystemLibrary::DrawDebugString(Pawn, TextLocation, Text, nullptr, FLinearColor::White, 15.0f);
	}
#endif

	return EBTNodeResult::Succeeded;
}

#if WITH_EDITOR
FString UENTQueryPaths::GetStaticDescription() const
{
	FString AIPathKeyDesc("invalid");
	if (AIPathKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		AIPathKeyDesc = AIPathKey.SelectedKeyName.ToString();
	}

	FString NavAreaKeyDesc("invalid");
	if (NavAreaKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		NavAreaKeyDesc = NavAreaKey.SelectedKeyName.ToString();
	}

	FString QueryText;
	if (bSearchForPaths && bSearchForNavArea)
	{
		QueryText = "Paths and NavArea";
	}
	else if (bSearchForPaths && !bSearchForNavArea)
	{
		QueryText = "Paths only";
	}
	else if (!bSearchForPaths && bSearchForNavArea)
	{
		QueryText = "Nav area only";
	}
	else
	{
		QueryText = "nothing";
	}

	return FString::Printf(TEXT("Query for to: %s (AIPathKey: %s, NavAreaKey: %s)"), *QueryText, *AIPathKeyDesc, *NavAreaKeyDesc);
}
#endif
