// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "ENTClearBlackboardVariable.generated.h"

UCLASS()
class ENTAI_API UENTClearBlackboardVariable : public UBTService
{
	GENERATED_BODY()
public:
	UENTClearBlackboardVariable();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void ClearValue(UBehaviorTreeComponent& OwnerComp);

	UPROPERTY(EditInstanceOnly, Category = "Bool")
	FBlackboardKeySelector BlackboardKey;

	/**
	 * @brief Select if the SetValue is executed at the start or at the end of the task attached
	 */
	UPROPERTY(EditInstanceOnly, Category = "Bool")
	bool bExecuteAtStart = true;
};
