// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ENTArtificialIntelligenceSubsystem.generated.h"

class UBlackboardComponent;

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTArtificialIntelligenceSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

#pragma region Defaults

protected:
	virtual void PostInitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual TStatId GetStatId() const override {return TStatId();}

	virtual void Tick(float DeltaTime) override;

#pragma endregion

#pragma region AI

protected:
	UPROPERTY()
	TArray<TObjectPtr<UObject>> AIList;

	/**
	 * @brief This actor is used for AI to track a moving noise
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Noise")
	TObjectPtr<AActor> NoiseActor;

	bool bIsNoiseActorAttached = false;

public:
	void AddAI(UObject* AIObject);

	void RemoveAI(UObject* AIObject);

	void AddNoiseActorToBlackboard(UBlackboardComponent* BlackboardComponent, const FName& KeyName) const;

	void UpdateNoiseActorLocation(const FVector& NewLocation) const;

	UFUNCTION(BlueprintCallable, Category = "Noise")
	void AttachNoiseActor(AActor* OtherActor);

	UFUNCTION(BlueprintCallable, Category = "Noise")
	void DetachNoiseActor();

#pragma endregion
};
