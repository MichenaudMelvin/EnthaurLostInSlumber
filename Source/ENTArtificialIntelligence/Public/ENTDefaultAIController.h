// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/ENTArtificialIntelligenceInterface.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "ENTDefaultAIController.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API AENTDefaultAIController : public AAIController, public IENTArtificialIntelligenceInterface
{
	GENERATED_BODY()

public:
	AENTDefaultAIController();

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	virtual void TickAI_Implementation(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName SpawnLocationKeyName = "SpawnLocation";

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Navigation")
	bool IsPointReachable(const FVector Point) const;
};
