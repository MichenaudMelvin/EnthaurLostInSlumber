// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "ENTCanReachLocation.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTCanReachLocation : public UBTDecorator
{
	GENERATED_BODY()

public:
	UENTCanReachLocation();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

private:
	void CheckAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Location")
	FBlackboardKeySelector Location;

	UPROPERTY(EditInstanceOnly, Category = "Location")
	bool bGroundLineTrace = true;

	UPROPERTY(EditInstanceOnly, Category = "Location", meta = (EditCondition = bGroundLineTrace, Units = cm))
	float GroundTraceLength = 100.0f;

	UPROPERTY(EditInstanceOnly, Category = "Location", meta = (EditCondition = bGroundLineTrace, Units = cm, ClampMin = 0.0f))
	float GroundQueryExtent = 100.0f;

	UPROPERTY(EditInstanceOnly, Category = "Location", meta = (EditCondition = bGroundLineTrace))
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditInstanceOnly, Category = "Location")
	bool bInverseCheck = false;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDebugDecorator = false;
#endif
};
