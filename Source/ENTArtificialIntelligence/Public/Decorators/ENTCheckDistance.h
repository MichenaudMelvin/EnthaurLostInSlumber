// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "ENTCheckDistance.generated.h"

UENUM(BlueprintType)
enum class EENTCheckMethod : uint8
{
	Equal,
	NotEqual,
	GreaterThanOrEqualTo,
	LessOrEqual,
	GreaterThan,
	LessThan,
};

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTCheckDistance : public UBTDecorator
{
	GENERATED_BODY()

public:
	UENTCheckDistance();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

private:
	void CheckAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Distance")
	FBlackboardKeySelector Actor;

	UPROPERTY(EditInstanceOnly, Category = "Distance", meta = (Units = cm))
	FValueOrBBKey_Float Distance;

	UPROPERTY(EditInstanceOnly, Category = "Behavior")
	EENTCheckMethod CheckMethod;
};
