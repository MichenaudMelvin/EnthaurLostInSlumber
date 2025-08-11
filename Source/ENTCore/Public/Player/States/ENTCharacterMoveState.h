// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTCharacterState.h"
#include "ENTCharacterMoveState.generated.h"

class UAkAudioEvent;
class UAkSwitchValue;

UCLASS(Abstract)
class ENTCORE_API UENTCharacterMoveState : public UENTCharacterState
{
	GENERATED_BODY()

public:
	UENTCharacterMoveState();

protected:
	virtual void StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = 0.0f, Units = "cm/s"))
	float MoveSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = 0.0f))
	float MoveAcceleration = 2048.0f;

	virtual void ApplyMovement(float DeltaTime);

	bool bMovementLocked = false;

#pragma region Noise

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise|Footsteps")
	TObjectPtr<UAkSwitchValue> FootstepsSpeedNoise;

	UPROPERTY(EditDefaultsOnly, Category = "Noise|Footsteps", meta = (ClampMin = 0.0f, ClampMax = 10.0f, UIMin = 0.0f, UIMax = 10.0f, Units = "s"))
	float FootstepsSpeedDelay = 0.4f;

	float FootstepsDelay = 0.0f;

	void PlayFootstepNoise(float DeltaTime);

#pragma endregion

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

	bool MovementLocked() const {return bMovementLocked;}
};
