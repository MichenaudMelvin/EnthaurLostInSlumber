// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ENTResetIgnoredCollisions.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTResetIgnoredCollisions : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	UENTResetIgnoredCollisions();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
