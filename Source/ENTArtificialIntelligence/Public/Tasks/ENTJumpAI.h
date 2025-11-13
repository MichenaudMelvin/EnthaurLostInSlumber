// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Components/TimelineComponent.h"
#include "ENTJumpAI.generated.h"

class AENTJumpSpline;

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTJumpAI : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UENTJumpAI();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

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

	/**
	 * @brief This vector is used when jumping on the ground to make the pawn face the location they goes
	 */
	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector GroundLookAtLocation;

	FTransform StartTransform;

	FTransform TargetTransform;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	TObjectPtr<UCurveFloat> JumpCurve;

	UPROPERTY(EditInstanceOnly, Category = "Path", meta = (Units = s, ClampMin = 0.0f))
	float JumpDuration = 1.0f;

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
