// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTCharacterState.h"
#include "ENTCharacterAnimState.generated.h"

UCLASS()
class ENTCORE_API UENTCharacterAnimState : public UENTCharacterState
{
	GENERATED_BODY()

public:
	UENTCharacterAnimState();

protected:
	EENTCharacterStateID PreviousState = EENTCharacterStateID::None;

	float AnimDuration = 0.0f;

	float StateDuration = 0.0f;

	virtual void StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;
};
