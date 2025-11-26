// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ENTTriggerAnimation.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTTriggerAnimation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UENTTriggerAnimation();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Animation")
	TObjectPtr<UAnimSequenceBase> AnimationToTrigger;

	UPROPERTY(EditInstanceOnly, Category = "Animation", meta = (Units = s, ClampMin = 0.0f))
	float AdditionalDuration = 0.0f;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> CurrentOwnerComp;

	UPROPERTY()
	TObjectPtr<APawn> CurrentPawn;

	UPROPERTY(EditInstanceOnly, Category = "Animation")
	bool bResetAnimationIfAborted = true;

	FTimerHandle AnimationTimerHandle;

	UPROPERTY(EditInstanceOnly, Category = "Task")
	FBlackboardKeySelector FinishCurrentTaskKey;

	UFUNCTION()
	void FinishTask();
};
