// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTAttackTarget.h"
#include "ENTHealthComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UENTAttackTarget::UENTAttackTarget()
{
	NodeName = "AttackTarget";
	Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTAttackTarget, Target), AActor::StaticClass());
}

void UENTAttackTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		Target.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UENTAttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CurrentBlackboard)
	{
		return EBTNodeResult::Failed;
	}

	UObject* TargetObject = CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(Target.GetSelectedKeyID());
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

#if WITH_EDITOR
FString UENTAttackTarget::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (Target.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		KeyDesc = Target.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Attack %s with %s damages"), *KeyDesc, *Damages.ToString());
}
#endif
