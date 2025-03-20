// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/SetBoolValue.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

USetBoolValue::USetBoolValue()
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;

	BoolKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(USetBoolValue, BoolKey));
}

void USetBoolValue::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		BoolKey.ResolveSelectedKey(*BBAsset);
	}
}

void USetBoolValue::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (bExecuteAtStart)
	{
		SetValue(OwnerComp);
	}
}

void USetBoolValue::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	if (!bExecuteAtStart)
	{
		SetValue(OwnerComp);
	}
}

void USetBoolValue::SetValue(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();

	if (!CurrentBlackboard)
	{
		return;
	}

	CurrentBlackboard->SetValue<UBlackboardKeyType_Bool>(BoolKey.GetSelectedKeyID(), bBoolValue);
}
