// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "Player/States/ENTCharacterState.h"
#include "ENTCheckPlayerState.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTCheckPlayerState : public UBTDecorator
{
	GENERATED_BODY()

public:
	UENTCheckPlayerState();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

private:
	void CheckAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Player")
	FBlackboardKeySelector Player;

	UPROPERTY(EditInstanceOnly, Category = "Player")
	EENTCharacterStateID TargetState;

	UPROPERTY(EditInstanceOnly, Category = "Player")
	bool bInverseCheck = false;
};
