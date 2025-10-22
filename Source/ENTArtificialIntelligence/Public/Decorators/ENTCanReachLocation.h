// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "ENTCanReachLocation.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTCanReachLocation : public UBTDecorator
{
	GENERATED_BODY()

public:
	UENTCanReachLocation();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	void CheckAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditInstanceOnly, Category = "Location")
	FBlackboardKeySelector Location;
};
