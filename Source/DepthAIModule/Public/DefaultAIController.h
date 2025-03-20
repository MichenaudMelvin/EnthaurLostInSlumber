// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "DefaultAIController.generated.h"

UCLASS()
class DEPTHAIMODULE_API ADefaultAIController : public AAIController
{
	GENERATED_BODY()

public:
	ADefaultAIController();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName SpawnLocationKeyName = "SpawnLocation";

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Navigation")
	bool IsPointReachable(const FVector Point) const;
};
