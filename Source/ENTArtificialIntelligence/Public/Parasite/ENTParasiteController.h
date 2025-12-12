// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTDefaultAIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "ENTParasiteController.generated.h"

struct FENTParaSiteData;
class UAIPerceptionComponent;

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API AENTParasiteController : public AENTDefaultAIController
{
	GENERATED_BODY()

public:
	AENTParasiteController();

protected:
	virtual void BeginPlay() override;

#if WITH_EDITORONLY_DATA
	virtual void Tick(float DeltaSeconds) override;
#endif

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void OnHearTarget(AActor* Actor, const FAIStimulus& Stimulus);

	void OnUnheardTarget(AActor* Actor);

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName PathIndexKeyName = "PathIndex";

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName PathDirectionKeyName = "PathDirection";

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName WalkOnFloorKeyName = "WalkOnFloor";

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName MoveLocationKeyName = "MoveLocation";

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName HeardNoiseKeyName = "HeardNoise";

	/**
	 * @brief This must be an object value instead of a vector
	 */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName NoiseLocationKeyName = "NoiseLocation";

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName NoiseInvestigatorKeyName = "NoiseInvestigator";

	UPROPERTY(EditDefaultsOnly, Category = "AI|Behavior", meta = (ClampMin = 0.0f, Units = "cm"))
	float HearingZTolerance = 100.0f;

#pragma region Saves

public:
	virtual void SaveControllerData(FENTAIData& AIData) override;

	virtual void LoadControllerData(const FENTAIData& AIData) override;

#pragma endregion
};
