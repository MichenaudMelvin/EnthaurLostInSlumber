// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTCharacterState.h"
#include "ENTCharacterStopState.generated.h"

UCLASS()
class ENTCORE_API UENTCharacterStopState : public UENTCharacterState
{
	GENERATED_BODY()

public:
	UENTCharacterStopState();

protected:
	virtual void StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Impulsion")
	float ImpulsionForce = 100.0f;
};
