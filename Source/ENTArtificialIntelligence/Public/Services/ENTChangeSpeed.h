// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "ENTChangeSpeed.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTChangeSpeed : public UBTService
{
	GENERATED_BODY()

public:
	UENTChangeSpeed();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	void ChangeSpeed(UBehaviorTreeComponent& OwnerComp);

	UPROPERTY(EditInstanceOnly, Category = "Bool")
	FValueOrBBKey_Float TargetSpeedValue;

	/**
	 * @brief Select if the SetValue is executed at the start or at the end of the task attached
	 */
	UPROPERTY(EditInstanceOnly, Category = "Bool")
	bool bExecuteAtStart = true;
};
