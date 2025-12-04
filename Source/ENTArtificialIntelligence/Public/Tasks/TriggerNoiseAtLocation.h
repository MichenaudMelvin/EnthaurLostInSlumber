// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TriggerNoiseAtLocation.generated.h"

class UAkAudioEvent;

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UTriggerNoiseAtLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTriggerNoiseAtLocation();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Noise")
	FBlackboardKeySelector NoiseLocation;

	UPROPERTY(EditInstanceOnly, Category = "Noise", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float NoiseLoudness = 1.0f;

	UPROPERTY(EditInstanceOnly, Category = "Noise", meta = (Units = cm))
	float NoiseRange = 1000.0f;

	UPROPERTY(EditInstanceOnly, Category = "Noise")
	FName NoiseTag = NAME_None;

	UPROPERTY(EditInstanceOnly, Category = "Noise")
	TObjectPtr<UAkAudioEvent> AudioEvent;
};
