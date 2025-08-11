// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTCharacterMoveState.h"
#include "ENTCharacterJumpState.generated.h"

UCLASS()
class ENTCORE_API UENTCharacterJumpState : public UENTCharacterMoveState
{
	GENERATED_BODY()

public:
	UENTCharacterJumpState();

protected:
	virtual void StateInit(UENTCharacterStateMachine* InStateMachine) override;

	virtual void StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;

	virtual void StateExit_Implementation(const EENTCharacterStateID& NextStateID) override;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Jump", meta = (ClampMin = 0.0f, Units = "cm/s"))
	float MaxHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Jump", meta = (ClampMin = 0.0f, Units = "s"))
	float Duration = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Jump", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float AirControl = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Jump", meta = (ClampMin = 0.0f))
	float FallingDeceleration = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkAudioEvent> JumpNoise;

private:
	float JumpVelocity = 0.0f;

	float JumpGravity = 0.0f;

	float PreviousGravityScale = 0.0f;
};
