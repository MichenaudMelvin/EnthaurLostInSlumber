// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "ENTAttackTarget.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTAttackTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UENTAttackTarget();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Attack")
	FValueOrBBKey_Float Damages;

	UPROPERTY(EditInstanceOnly, Category = "Attack")
	FBlackboardKeySelector Target;
};
