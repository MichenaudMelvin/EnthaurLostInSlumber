// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "ENTUpdatePathIndex.generated.h"


UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTUpdatePathIndex : public UBTService
{
	GENERATED_BODY()

public:
	UENTUpdatePathIndex();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector AIPath;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector AINextPath;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector NextPathLocation;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector PathIndex;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	FBlackboardKeySelector PathDirection;

	UPROPERTY(EditInstanceOnly, Category = "Path")
	bool bCanStopBehaviorIfThePathDoesNotLoop = true;
};
