// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "UpdatePathIndex.generated.h"


UCLASS()
class DEPTHAIMODULE_API UUpdatePathIndex : public UBTService
{
	GENERATED_BODY()

public:
	UUpdatePathIndex();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector AIPath;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector PathIndex;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector PathDirection;
};
