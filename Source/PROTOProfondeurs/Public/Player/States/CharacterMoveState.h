// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"
#include "CharacterMoveState.generated.h"

UCLASS(Abstract)
class PROTOPROFONDEURS_API UCharacterMoveState : public UCharacterState
{
	GENERATED_BODY()

public:
	UCharacterMoveState();

protected:
	virtual void StateEnter_Implementation(const ECharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = 0.0f, Units = "cm/s"))
	float MoveSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = 0.0f))
	float MoveAcceleration = 2048.0f;

public:
	float GetMoveSpeed() const {return MoveSpeed;}

	float GetMoveAcceleration() const {return MoveAcceleration;}
};
