// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "ENTComputeClosestReachablePoint.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTComputeClosestReachablePoint : public UBTService
{
	GENERATED_BODY()

public:
	UENTComputeClosestReachablePoint();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditInstanceOnly, Category = "Point")
	FBlackboardKeySelector Point;

	/**
	 * @brief Can be the same value as point (if a FVector only)
	 */
	UPROPERTY(EditInstanceOnly, Category = "Point")
	FBlackboardKeySelector ClosestPoint;

	UPROPERTY(EditInstanceOnly, Category = "Point")
	FVector QueryExtent = FVector(100.0f);

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDebugService = false;
#endif
};
