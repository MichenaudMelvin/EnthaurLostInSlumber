// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Components/TimelineComponent.h"
#include "ENTJumpTask.generated.h"

class AENTJumpSpline;

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTJumpTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UENTJumpTask();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditInstanceOnly, Category = "Jump")
	FBlackboardKeySelector JumpLocationKey;

	/**
	 * @brief This vector is used when jumping on the ground to make the pawn face the location they goes
	 */
	UPROPERTY(EditInstanceOnly, Category = "Jump")
	FBlackboardKeySelector GroundLookAtLocation;

	FTransform StartTransform;

	FTransform TargetTransform;

	UPROPERTY(EditInstanceOnly, Category = "Jump")
	TObjectPtr<UCurveFloat> JumpCurve;

	UPROPERTY(EditInstanceOnly, Category = "Jump", meta = (Units = s, ClampMin = 0.0f))
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
	virtual void MovementUpdate(float Alpha);

	UFUNCTION()
	virtual void FinishTask();

	virtual void SetTargetTransform();

	FVector GetTargetForwardVector() const;
};
