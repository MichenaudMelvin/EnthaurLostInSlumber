// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "LookAt.generated.h"

UCLASS()
class DEPTHAIMODULE_API ULookAt : public UBTTaskNode{
	GENERATED_BODY()

public:
	ULookAt();

private:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditInstanceOnly, Category = "Rotation")
	FBlackboardKeySelector LookAtPosition;

	UPROPERTY(EditInstanceOnly, Category = "Rotation")
	bool bUseControlRotation = false;

	UPROPERTY(EditInstanceOnly, Category = "Rotation")
	bool bIgnorePitchRotation = true;

	UPROPERTY(EditInstanceOnly, Category = "Rotation")
	bool bInstantRotation = false;

	UPROPERTY(EditInstanceOnly, Category = "Rotation", meta = (ClampMin = 0.0f, EditCondition = "!bInstantRotation"))
	float RotationSpeed = 5.0f;

	UPROPERTY(EditInstanceOnly, Category = "Rotation", meta = (ClampMin = 0.0f, ClampMax = 360.0f, UIMin = 0.0f, UIMax = 360.0f, EditCondition = "!bInstantRotation"))
	float RotationTolerance = 1.0f;

private:
	FRotator TargetRotation = FRotator::ZeroRotator;
};