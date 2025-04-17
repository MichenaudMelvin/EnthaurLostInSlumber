// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Components/TimelineComponent.h"
#include "FollowAIPath.generated.h"

UCLASS()
class DEPTHAIMODULE_API UFollowAIPath : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UFollowAIPath();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector AIPath;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector PathIndex;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector WalkOnFloor;

	UPROPERTY(EditInstanceOnly, Category = "Path", meta = (ClampMin = 0.0f, Units = "cm"))
	float Tolerance = 0.1f;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	bool bRotateWithMovement = true;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	TObjectPtr<UCurveFloat> MovementCurve;

	FVector TargetLocation;

#pragma region WalkOnWall

protected:
	UPROPERTY()
	TObjectPtr<APawn> CurrentPawn;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> CurrentOwnerComp;

	FVector StartLocation;

	FTimeline MovementTimeline;

	UFUNCTION()
	void MovementUpdate(float Alpha);

	UFUNCTION()
	void FinishTask();
#pragma endregion
};
