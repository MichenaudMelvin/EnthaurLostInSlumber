// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ChangePawnGravity.generated.h"

/**
 * @brief Pawn must have a UGravityPawnMovement to works
 */
UCLASS()
class DEPTHAIMODULE_API UChangePawnGravity : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditInstanceOnly, Category = "Gravity")
	float NewGravityScale = 1.0f;

	UPROPERTY(EditInstanceOnly, Category = "Gravity")
	FVector NewGravityDirection = FVector(0.0f, 0.0f, -1.0f);
};
