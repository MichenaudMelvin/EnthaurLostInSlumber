// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterMoveState.h"
#include "CharacterCrouchState.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UCharacterCrouchState : public UCharacterMoveState
{
	GENERATED_BODY()

public:
	UCharacterCrouchState();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Crouch", meta = (ClampMin = 0.0f, Units = "cm"))
	float CrouchHalfHeight = 44.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Crouch", meta = (ClampMin = 0.0f, Units = "cm"))
	float CameraHeight = 16.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Crouch", meta = (ClampMin = 0.0f, Units = "cm"))
	float Duration = 1.0f;

	virtual void StateInit(UCharacterStateMachine* InStateMachine) override;

	virtual void StateEnter_Implementation(const ECharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;

	virtual void StateExit_Implementation(const ECharacterStateID& NextStateID) override;

	void LerpCrouch(float DeltaTime, bool bStandUp);

	bool CanStandUp() const;

private:
	float DefaultHalfHeight = 0.0f;

	FVector DefaultCameraLocation = FVector::ZeroVector;

	float DefaultSpeed = 0.0f;

	float CurrentDuration = 0.0f;

	float DefaultCameraHeight = 0.0f;
};
