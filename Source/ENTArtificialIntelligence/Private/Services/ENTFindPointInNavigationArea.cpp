// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/ENTFindPointInNavigationArea.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Path/ENTNavigationArea.h"

UENTFindPointInNavigationArea::UENTFindPointInNavigationArea()
{
	NodeName = "FindPointInNavigationArea";
	bNotifyBecomeRelevant = true;
	bCreateNodeInstance = true;

	ForceInstancing(true);

	NavigationArea.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTFindPointInNavigationArea, NavigationArea), AENTNavigationArea::StaticClass());
	RandomLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTFindPointInNavigationArea, RandomLocationKey));
}

void UENTFindPointInNavigationArea::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		NavigationArea.ResolveSelectedKey(*BBAsset);
		RandomLocationKey.ResolveSelectedKey(*BBAsset);
	}
}

void UENTFindPointInNavigationArea::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return;
	}

	UObject* KeyObject = BlackboardComponent->GetValue<UBlackboardKeyType_Object>(NavigationArea.GetSelectedKeyID());
	if (!KeyObject)
	{
		return;
	}

	AENTNavigationArea* NavArea = Cast<AENTNavigationArea>(KeyObject);
	if (!NavArea)
	{
		return;
	}

	FVector TargetPoint = NavArea->GetRandomPointInArea();
	BlackboardComponent->SetValue<UBlackboardKeyType_Vector>(RandomLocationKey.GetSelectedKeyID(), TargetPoint);

#if WITH_EDITORONLY_DATA
	if (bDebugService)
	{
		UKismetSystemLibrary::DrawDebugPoint(OwnerComp.GetOwner(), TargetPoint, 15.0f, DebugPointColor, 5.0f);
	}
#endif
}

#if WITH_EDITOR
FString UENTFindPointInNavigationArea::GetStaticDescription() const
{
	FString NavigationAreaDesc("invalid");
	if (NavigationArea.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		NavigationAreaDesc = NavigationArea.SelectedKeyName.ToString();
	}

	FString RandomLocationKeyDesc("invalid");
	if (RandomLocationKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		RandomLocationKeyDesc = RandomLocationKey.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("%s will set %s as random location"), *NavigationAreaDesc, *RandomLocationKeyDesc);
}
#endif
