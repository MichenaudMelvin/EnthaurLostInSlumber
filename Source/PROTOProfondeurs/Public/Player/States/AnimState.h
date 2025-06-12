// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"
#include "AnimState.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UAnimState : public UCharacterState
{
	GENERATED_BODY()

public:
	UAnimState();

protected:
	ECharacterStateID PreviousState = ECharacterStateID::None;

	float AnimDuration = 0.0f;

	float StateDuration = 0.0f;

	virtual void StateEnter_Implementation(const ECharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;
};
