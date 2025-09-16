// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "ENTSetRandomLocation.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTSetRandomLocation : public UBTService
{
	GENERATED_BODY()

public:
	UENTSetRandomLocation();

private:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void SetOwner(AActor* InActorOwner) override;

	UPROPERTY(EditInstanceOnly, Category = "RandomLocation", meta = (DisplayName = "RandomLocation"))
	FBlackboardKeySelector RandomLocationKey;

	UPROPERTY()
	bool bUsePawnLocation = false;

	/**
	 * @brief Origin of the point, disabling it will take the location of the pawn as the origin
	 */
	UPROPERTY(EditInstanceOnly, Category = "RandomLocation", meta = (EditCondition = bUsePawnLocation))
	FBlackboardKeySelector Origin;

	UPROPERTY(EditInstanceOnly, Category = "RandomLocation", meta = (Units = "cm"))
	float Radius = 500.0f;

	UPROPERTY(EditInstanceOnly, Category = "RandomLocation")
	TSubclassOf<UNavigationQueryFilter> NavFilter;

	UPROPERTY(Transient)
	TObjectPtr<AActor> ActorOwner;

	UPROPERTY(EditInstanceOnly, Category = "RandomLocation")
	TObjectPtr<ANavigationData> NavData;
};
