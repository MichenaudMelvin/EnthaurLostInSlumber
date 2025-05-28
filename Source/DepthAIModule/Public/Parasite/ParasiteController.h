// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultAIController.h"
#include "ParasitePawn.h"
#include "Perception/AIPerceptionTypes.h"
#include "ParasiteController.generated.h"

class UAIPerceptionComponent;

UCLASS()
class DEPTHAIMODULE_API AParasiteController : public ADefaultAIController
{
	GENERATED_BODY()

public:
	AParasiteController();

protected:
	virtual void BeginPlay() override;

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

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName NoiseInvestigatorKeyName = "NoiseInvestigator";

	UPROPERTY(EditDefaultsOnly, Category = "AI|Behavior", meta = (ClampMin = 0.0f, Units = "cm"))
	float HearingZTolerance = 100.0f;

public:
	virtual void SaveBlackBoardValues(FParaSiteData& AIData);

	virtual void LoadBlackboardValues(const FParaSiteData& AIData);
};
