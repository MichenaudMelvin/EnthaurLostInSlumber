// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "ENTLerpRotation.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTLerpRotation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UENTLerpRotation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditInstanceOnly, Category = "Rotation")
	FValueOrBBKey_Rotator TargetRotation = FRotator::ZeroRotator;

	UPROPERTY(EditInstanceOnly, Category = "Rotation")
	FValueOrBBKey_Float LerpSpeed = 1.0f;

	UPROPERTY(EditInstanceOnly, Category = "Rotation")
	FValueOrBBKey_Float Tolerance = 0.1f;
};
