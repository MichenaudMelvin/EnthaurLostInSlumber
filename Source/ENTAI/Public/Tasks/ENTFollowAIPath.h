// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Components/TimelineComponent.h"
#include "ENTFollowAIPath.generated.h"

/**
 * @brief If the pawn can walk on the floor it will return a TargetLocation, else it will lerp pawn location
 */
UCLASS()
class ENTAI_API UENTFollowAIPath : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UENTFollowAIPath();

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

	FVector TargetLocation;

#pragma region WalkOnFloor

protected:
	UPROPERTY(EditInstanceOnly, Category = "Path|Floor", meta = (DisplayName = "TargetLocation"))
	FBlackboardKeySelector TargetKeyLocation;

#pragma endregion

#pragma region WalkOnWall

protected:
	UPROPERTY()
	TObjectPtr<APawn> CurrentPawn;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> CurrentOwnerComp;

	UPROPERTY(EditInstanceOnly, Category = "Path|Wall")
	TObjectPtr<UCurveFloat> MovementCurve;

	FVector StartLocation;

	FTimeline MovementTimeline;

	UFUNCTION()
	void MovementUpdate(float Alpha);

	UFUNCTION()
	void FinishTask();
#pragma endregion

#pragma region Debug

#if WITH_EDITORONLY_DATA
protected:
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDebugTask = false;

	UPROPERTY(EditInstanceOnly, Category = "Debug", meta = (ClampMin = 1.0f, Units = "cm"))
	float DebugPointSize = 15.0f;
#endif

#pragma endregion
};
