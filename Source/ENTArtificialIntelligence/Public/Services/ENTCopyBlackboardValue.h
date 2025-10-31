// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "ENTCopyBlackboardValue.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTCopyBlackboardValue : public UBTService
{
	GENERATED_BODY()

public:
	UENTCopyBlackboardValue();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	void CopyValue(UBehaviorTreeComponent& OwnerComp) const;

	UPROPERTY(EditInstanceOnly, Category = "Service")
	FBlackboardKeySelector SourceBlackboardKey;

	UPROPERTY(EditInstanceOnly, Category = "Service")
	FBlackboardKeySelector DestinationBlackboardKey;

	/**
	 * @brief Select if the SetValue is executed at the start or at the end of the task attached
	 */
	UPROPERTY(EditInstanceOnly, Category = "Service")
	bool bExecuteAtStart = true;
};
