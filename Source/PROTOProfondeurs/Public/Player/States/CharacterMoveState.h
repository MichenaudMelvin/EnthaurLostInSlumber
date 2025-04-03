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

	virtual void ApplyMovement();

	virtual void UpdateCameraSteering(float DeltaTime) override;

	bool bMovementLocked = false;

public:
	float GetMoveSpeed() const {return MoveSpeed;}

	float GetMoveAcceleration() const {return MoveAcceleration;}

	/**
	 * @brief Update the current speed of the Character
	 * @param NewSpeed is temporary, everytime you change to this state, Character speed will be the MoveSpeed
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetNewSpeed(const float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void LockMovement(const bool bLockMovement) {bMovementLocked = bLockMovement;}
};
