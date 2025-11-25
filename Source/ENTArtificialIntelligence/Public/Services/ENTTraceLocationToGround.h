// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "ENTTraceLocationToGround.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTTraceLocationToGround : public UBTService
{
	GENERATED_BODY()

public:
	UENTTraceLocationToGround();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	void TraceToGround(UBehaviorTreeComponent& OwnerComp);

	UPROPERTY(EditInstanceOnly, Category = "Location")
	FBlackboardKeySelector LocationKey;

	UPROPERTY(EditInstanceOnly, Category = "Collision")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditInstanceOnly, Category = "Collision", meta = (Units = cm, ClampMin = 0.0f))
	float GroundTraceLength = 100.0f;

	/**
	 * @brief Select if the SetValue is executed at the start or at the end of the task attached
	 */
	UPROPERTY(EditInstanceOnly, Category = "Bool")
	bool bExecuteAtStart = true;
};
