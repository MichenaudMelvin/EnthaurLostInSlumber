// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTCharacterState.h"
#include "ENTCharacterIdleState.generated.h"

UCLASS()
class ENTCORE_API UENTCharacterIdleState : public UENTCharacterState
{
	GENERATED_BODY()

public:
	UENTCharacterIdleState();

protected:
	virtual void StateInit(UENTCharacterStateMachine* InStateMachine) override;

	virtual void StateTick_Implementation(float DeltaTime) override;
};
