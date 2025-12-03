// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTCharacterState.h"
#include "Components/TimelineComponent.h"
#include "ENTCharacterLookAtState.generated.h"

UCLASS()
class ENTCORE_API UENTCharacterLookAtState : public UENTCharacterState
{
	GENERATED_BODY()

public:
	UENTCharacterLookAtState();

protected:
	virtual void StateInit(UENTCharacterStateMachine* InStateMachine) override;

	virtual void StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;

	UFUNCTION()
	void LookAtUpdate(float Alpha);

	UFUNCTION()
	void FinishLookAt();

	FTimeline LookAtTimeline;

	FRotator StartRotation;

	FRotator TargetRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Curve")
	TObjectPtr<UCurveFloat> StateCurve;

	UPROPERTY(EditDefaultsOnly, Category = "State")
	EENTCharacterStateID NextState = EENTCharacterStateID::Idle;

	float StateDuration = 1.0f;

public:
	void SetLookAtLocation(const FVector& InLocationToLookAt, float InStateDuration, UCurveFloat* CurveFloat, EENTCharacterStateID InNextState);
};
