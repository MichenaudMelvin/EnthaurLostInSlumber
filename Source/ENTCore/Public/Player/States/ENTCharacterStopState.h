// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTCharacterState.h"
#include "ENTCharacterStopState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishStop);

UCLASS()
class ENTCORE_API UENTCharacterStopState : public UENTCharacterState
{
	GENERATED_BODY()

public:
	UENTCharacterStopState();

protected:
	virtual void StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;

	float StopDuration = -1.0f;

	float StopTime = 0.0f;

	/**
	 * @brief Fired when StopTime reach the StopDuration, not when switching state from the state machine
	 */
	UPROPERTY(BlueprintAssignable, Category = "Stop")
	FOnFinishStop OnFinishStop;

public:
	void SetStopDuration(float InStopDuration);
};
