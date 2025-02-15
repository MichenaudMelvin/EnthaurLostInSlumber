// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterMoveState.h"
#include "CharacterJumpState.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UCharacterJumpState : public UCharacterMoveState
{
	GENERATED_BODY()

public:
	UCharacterJumpState();

protected:
	virtual void StateInit(UCharacterStateMachine* InStateMachine) override;

	virtual void StateEnter_Implementation(const ECharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Jump", meta = (ClampMin = 0.0f, Units = "cm/s"))
	float MaxHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Jump", meta = (ClampMin = 0.0f, Units = "s"))
	float Duration = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Jump", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float AirControl = 1.0f;

private:
	float JumpVelocity = 0.0f;

	float JumpGravity = 0.0f;
};
