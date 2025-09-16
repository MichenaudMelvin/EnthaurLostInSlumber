// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ENTArtificialIntelligenceSubsystem.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTArtificialIntelligenceSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

#pragma region Defaults

protected:
	virtual void PostInitialize() override;

	virtual TStatId GetStatId() const override {return TStatId();}

	virtual void Tick(float DeltaTime) override;

#pragma endregion

#pragma region AI

protected:
	UPROPERTY()
	TArray<TObjectPtr<UObject>> AIList;

public:
	void AddAI(UObject* AIObject);

	void RemoveAI(UObject* AIObject);

#pragma endregion
};
