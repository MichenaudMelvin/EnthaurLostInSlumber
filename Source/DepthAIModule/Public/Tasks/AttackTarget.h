// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "AttackTarget.generated.h"

UCLASS()
class DEPTHAIMODULE_API UAttackTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UAttackTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditInstanceOnly, Category = "Attack")
	FValueOrBBKey_Float Damages;

	UPROPERTY(EditInstanceOnly, Category = "Attack")
	FBlackboardKeySelector Target;
};
