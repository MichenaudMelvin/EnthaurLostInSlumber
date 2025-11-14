// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTJumpTask.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ENTSplineJumpTask.generated.h"

class AENTJumpSpline;

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTSplineJumpTask : public UENTJumpTask
{
	GENERATED_BODY()

public:
	UENTSplineJumpTask();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector AIPath;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector PathDirection;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector DoesWalkOnFloor;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	bool bJumpOnTheGround = false;

	virtual void SetTargetTransform() override;

	virtual void FinishTask() override;
};
