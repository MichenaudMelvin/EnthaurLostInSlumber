// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ENTQueryPaths.generated.h"

/**
 * @brief Set the closest path or navigation area 
 */
UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTQueryPaths : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UENTQueryPaths();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Query", DisplayName = "AIPath")
	FBlackboardKeySelector AIPathKey;

	UPROPERTY(EditInstanceOnly, Category = "Query", DisplayName = "NavArea")
	FBlackboardKeySelector NavAreaKey;

	UPROPERTY(EditInstanceOnly, Category = "Query")
	bool bSearchForPaths = true;

	UPROPERTY(EditInstanceOnly, Category = "Query")
	bool bSearchForNavArea = true;

	UPROPERTY(EditInstanceOnly, Category = "Query")
	bool bIgnoreCurrentValue = true;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDebugTask = false;
#endif
};
