// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ENTOverrideParasiteVelocity.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTOverrideParasiteVelocity : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UENTOverrideParasiteVelocity();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Velocity")
	FVector OverridenVelocity;
};
