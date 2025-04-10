// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"
#include "CharacterStopState.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UCharacterStopState : public UCharacterState
{
	GENERATED_BODY()

public:
	UCharacterStopState();

protected:
	virtual void StateEnter_Implementation(const ECharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Impulsion")
	float ImpulsionForce = 100.0f;
};
