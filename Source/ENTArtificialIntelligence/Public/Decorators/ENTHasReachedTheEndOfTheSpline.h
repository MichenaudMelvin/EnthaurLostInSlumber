// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "ENTHasReachedTheEndOfTheSpline.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTHasReachedTheEndOfTheSpline : public UBTDecorator
{
	GENERATED_BODY()

public:
	UENTHasReachedTheEndOfTheSpline();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

private:
	void CheckAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector AIPath;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector PathIndex;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector PathDirection;

	UPROPERTY(EditInstanceOnly, Category = "Path", meta = (Units = cm))
	float Tolerance = 5.0f;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	bool bInverseCheck = false;
};
