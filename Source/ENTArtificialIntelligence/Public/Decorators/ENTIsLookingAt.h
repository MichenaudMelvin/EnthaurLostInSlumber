// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "ENTIsLookingAt.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTIsLookingAt : public UBTDecorator
{
	GENERATED_BODY()

public:
	UENTIsLookingAt();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

private:
	void CheckAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "LookingAt")
	FBlackboardKeySelector TargetLocation;

	UPROPERTY(EditInstanceOnly, Category = "LookingAt")
	bool bInverseCheck = false;

	UPROPERTY(EditInstanceOnly, Category = "LookingAt")
	float Tolerance = 0.5f;
};
