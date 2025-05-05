// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "MoveToWithRotation.generated.h"

UCLASS()
class DEPTHAIMODULE_API UMoveToWithRotation : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UMoveToWithRotation();

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditInstanceOnly, Category = "Ground", meta = (ClampMin = 0.0f, Units = "cm"))
	float GroundTraceLength = 25.0f;
};
