// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SetBoolValue.generated.h"

UCLASS()
class DEPTHAIMODULE_API USetBoolValue : public UBTService
{
	GENERATED_BODY()

public:
	USetBoolValue();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditInstanceOnly, Category = "Bool")
	FBlackboardKeySelector BoolKey;

	UPROPERTY(EditInstanceOnly, Category = "Bool")
	bool bBoolValue = true;

	/**
	 * @brief Select if the SetValue is executed at the start or at the end of the task attached
	 */
	UPROPERTY(EditInstanceOnly, Category = "Bool")
	bool bExecuteAtStart = true;

	void SetValue(UBehaviorTreeComponent& OwnerComp);
};
