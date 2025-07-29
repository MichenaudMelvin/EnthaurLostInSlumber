// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/AttackTarget.h"

#include "ENTHealthComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

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

	AActor* TargetActor = Cast<AActor>(TargetObject);
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	UENTHealthComponent* HealthComponent = TargetActor->GetComponentByClass<UENTHealthComponent>();
	if (!HealthComponent)
	{
		return EBTNodeResult::Failed;
	}

	HealthComponent->TakeDamages(Damages.GetValue(OwnerComp));

	return EBTNodeResult::Succeeded;
}
