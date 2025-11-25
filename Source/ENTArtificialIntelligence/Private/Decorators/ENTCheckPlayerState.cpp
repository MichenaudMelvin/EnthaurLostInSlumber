// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorators/ENTCheckPlayerState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Player/ENTDefaultCharacter.h"

UENTCheckPlayerState::UENTCheckPlayerState()
{
	NodeName = "CheckPlayerState";
	Player.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTCheckPlayerState, Player), AActor::StaticClass());
	bNotifyTick = true;

	ForceInstancing(true);
}

void UENTCheckPlayerState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	CheckAbort(OwnerComp, NodeMemory);
}

void UENTCheckPlayerState::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		Player.ResolveSelectedKey(*BBAsset);
	}
}

void UENTCheckPlayerState::CheckAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
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

bool UENTCheckPlayerState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();

	if (!CurrentBlackboard)
	{
		return false;
	}

	const UObject* Object = CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(Player.SelectedKeyName);

	if (!Object)
	{
		return false;
	}

	const AENTDefaultCharacter* Character = Cast<AENTDefaultCharacter>(Object);

	if (!Character)
	{
		return false;
	}

	return bInverseCheck != Character->CompareCurrentState(TargetState);
}

#if WITH_EDITOR
FString UENTCheckPlayerState::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (Player.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		KeyDesc = Player.SelectedKeyName.ToString();
	}

	if (bInverseCheck)
	{
		return FString::Printf(TEXT("Is CurrentState of %s != %s"), *KeyDesc, *StateToString(TargetState));
	}
	else
	{
		return FString::Printf(TEXT("Is CurrentState of %s == %s"), *KeyDesc, *StateToString(TargetState));
	}
}
#endif
