// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Navigation/ENTExcludeJumpNavAreaFilter.h"
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

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Point")
	FBlackboardKeySelector Point;

	/**
	 * @brief Can be the same value as point (if a FVector only)
	 */
	UPROPERTY(EditInstanceOnly, Category = "Point")
	FBlackboardKeySelector ClosestPoint;

	UPROPERTY(EditInstanceOnly, Category = "Point", meta = (Units = cm, ClampMin = 0.0f))
	float QueryExtent = 100.0f;

	UPROPERTY(EditInstanceOnly, Category = "Point")
	TSubclassOf<UNavigationQueryFilter> FilterClass = UENTExcludeJumpNavAreaFilter::StaticClass();

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDebugService = false;
#endif
};
