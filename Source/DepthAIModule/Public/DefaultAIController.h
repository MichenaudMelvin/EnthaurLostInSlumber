// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveAIData.h"
#include "Interfaces/AIInterface.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "DefaultAIController.generated.h"

UCLASS()
class DEPTHAIMODULE_API ADefaultAIController : public AAIController, public IAIInterface
{
	GENERATED_BODY()

public:
	ADefaultAIController();

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
