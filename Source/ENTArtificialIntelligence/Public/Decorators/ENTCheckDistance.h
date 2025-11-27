// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "ENTCheckDistance.generated.h"

UENUM(BlueprintType)
enum class EENTCheckMethod : uint8
{
	Equal,
	NotEqual,
	GreaterThanOrEqualTo,
	LessOrEqual,
	GreaterThan,
	LessThan,
};

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTCheckDistance : public UBTDecorator
{
	GENERATED_BODY()

public:
	UENTCheckDistance();

protected:
	virtual void OnNodeProcessed(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult) override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

private:
	void CheckAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Distance")
	FBlackboardKeySelector Actor;

	UPROPERTY(EditInstanceOnly, Category = "Distance", meta = (Units = cm))
	FValueOrBBKey_Float Distance;

	UPROPERTY(EditInstanceOnly, Category = "Behavior")
	EENTCheckMethod CheckMethod;

	FDateTime LastTimeTriggered;

	bool bHasTriggerAlreadyOnce = false;

	void ComputeDistance(UBehaviorTreeComponent& OwnerComp);

	bool bDistanceResult = false;

	UPROPERTY(EditInstanceOnly, Category = "Collision")
	FValueOrBBKey_Bool CollisionTest;

	UPROPERTY(EditInstanceOnly, Category = "Collision")
	bool bInverseCollisionTestValue;

	/**
	 * @brief How long the CollisionTest should succeed to allow decorator to pass, 0 means instant
	 */
	UPROPERTY(EditInstanceOnly, Category = "Collision", meta = (Units = s, ClampMin = 0.0f))
	float CollisionTestDuration = 5.0f;

	float CollisionTestTime = 0.0f;

	bool bCollisionTestResult = false;

	/**
	 * @brief Start as true
	 */
	bool bHasTimerAlreadyFailed = true;

	bool bHasTimerAlreadySucceed = false;

	/**
	 * @brief Only used if CollisionTest is true 
	 */
	UPROPERTY(EditInstanceOnly, Category = "Collision")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	bool DoCollisionTest(const UBehaviorTreeComponent& OwnerComp) const;

	/**
	 * @brief 
	 * @param OwnerComp 
	 * @return true if succeeded
	 */
	bool TraceCollisionTest(UBehaviorTreeComponent& OwnerComp) const;

	void ComputeCollisionTestDuration(float DeltaTime, bool bSucceedCollisionTest);

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDebugDecorator = false;
#endif
};
