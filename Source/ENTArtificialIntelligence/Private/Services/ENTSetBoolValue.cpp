// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/ENTSetBoolValue.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

UENTSetBoolValue::UENTSetBoolValue()
{
	NodeName = "SetBoolValue";
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;

	ForceInstancing(true);

	BoolKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UENTSetBoolValue, BoolKey));
}

void UENTSetBoolValue::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		BoolKey.ResolveSelectedKey(*BBAsset);
	}
}

void UENTSetBoolValue::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (bExecuteAtStart)
	{
		SetValue(OwnerComp);
	}
}

void UENTSetBoolValue::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	if (!bExecuteAtStart)
	{
		SetValue(OwnerComp);
	}
}

#if WITH_EDITOR
FString UENTSetBoolValue::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (BoolKey.SelectedKeyType == UBlackboardKeyType_Bool::StaticClass())
	{
		KeyDesc = BoolKey.SelectedKeyName.ToString();
	}

	FString boolValue = bBoolValue ? FString("true") : FString("false");
	return FString::Printf(TEXT("Set %s as %s"), *KeyDesc, *boolValue);

}
#endif

void UENTSetBoolValue::SetValue(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();

	if (!CurrentBlackboard)
	{
		return;
	}

	CurrentBlackboard->SetValue<UBlackboardKeyType_Bool>(BoolKey.GetSelectedKeyID(), bBoolValue);
}
