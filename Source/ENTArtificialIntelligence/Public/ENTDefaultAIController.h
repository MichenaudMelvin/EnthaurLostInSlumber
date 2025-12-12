// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/ENTArtificialIntelligenceInterface.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "ENTDefaultAIController.generated.h"

class UENTWorldSave;
struct FENTAIData;

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API AENTDefaultAIController : public AAIController, public IENTArtificialIntelligenceInterface
{
	GENERATED_BODY()

public:
	AENTDefaultAIController();

protected:
	virtual void BeginPlay() override;

	virtual void StartupActions();

#if WITH_EDITORONLY_DATA
	virtual void Tick(float DeltaSeconds) override;
#endif

	virtual void Destroyed() override;

	virtual void TickAI_Implementation(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName SpawnLocationKeyName = "SpawnLocation";

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName SpawnRotationKeyName = "SpawnRotation";

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	bool bIsBehaviorTreeRunning = false;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "AI|Debug")
	bool bDebugAI = false;
#endif

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Navigation")
	bool IsPointReachable(const FVector& Point, const FVector& Extent = FVector(5.0f)) const;

	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void RunCurrentBehaviorTree();

	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void StopBehaviorTree();

#pragma region Saves

public:
	UFUNCTION()
	void LoadingActions(UENTWorldSave* WorldSave);

	virtual void SaveControllerData(FENTAIData& AIData);

	virtual void LoadControllerData(const FENTAIData& AIData);
#pragma endregion
};
