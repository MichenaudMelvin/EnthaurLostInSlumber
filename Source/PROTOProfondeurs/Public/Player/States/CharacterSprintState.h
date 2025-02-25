// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterMoveState.h"
#include "CharacterSprintState.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UCharacterSprintState : public UCharacterMoveState
{
	GENERATED_BODY()

public:
	UCharacterSprintState();

protected:
	virtual void StateTick_Implementation(float DeltaTime) override;
};
