// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Components/TimelineComponent.h"
#include "ENTJumpToNextPath.generated.h"

class AENTJumpSpline;

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTJumpToNextPath : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UENTJumpToNextPath();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector AIPath;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector AINextPath;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector DoesWalkOnFloor;

	FTransform StartTransform;

	FTransform TargetTransform;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	TObjectPtr<UCurveFloat> JumpCurve;

	FTimeline JumpTimeline;

	UPROPERTY()
	TObjectPtr<APawn> CurrentPawn;

	UPROPERTY()
	UBlackboardComponent* CurrentBlackboardComponent;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> CurrentOwnerComp;

	UPROPERTY()
	TObjectPtr<AENTJumpSpline> JumpSpline;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDebugTask = false;
#endif

	UFUNCTION()
	void MovementUpdate(float Alpha);

	UFUNCTION()
	void FinishTask();
};
