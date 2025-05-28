// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "IgnoreFloorCollisions.generated.h"

UCLASS()
class DEPTHAIMODULE_API UIgnoreFloorCollisions : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditInstanceOnly, Category = "Collisions")
	TArray<TEnumAsByte<EObjectTypeQuery>> CheckObjectTypes;

	UPROPERTY(EditInstanceOnly, Category = "Collisions")
	FValueOrBBKey_Float TraceLength = 50.0f;
};
