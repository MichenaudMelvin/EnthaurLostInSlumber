// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/AttackTarget.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Player/FirstPersonCharacter.h"

UAttackTarget::UAttackTarget()
{
	Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UAttackTarget, Target), AActor::StaticClass());
}

EBTNodeResult::Type UAttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CurrentBlackboard)
	{
		return EBTNodeResult::Failed;
	}

	UObject* TargetObject = CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(Target.SelectedKeyName);
	if (!TargetObject)
	{
		return EBTNodeResult::Failed;
	}

	// hardcoded, would be nice to do a health component or an interface
	AFirstPersonCharacter* FirstPersonCharacter = Cast<AFirstPersonCharacter>(TargetObject);
	if (!FirstPersonCharacter)
	{
		return EBTNodeResult::Failed;
	}

	FirstPersonCharacter->KillPlayer();

	return EBTNodeResult::Succeeded;
}
