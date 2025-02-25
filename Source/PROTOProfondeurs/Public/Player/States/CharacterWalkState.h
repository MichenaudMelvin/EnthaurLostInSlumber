// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterMoveState.h"
#include "CharacterWalkState.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UCharacterWalkState : public UCharacterMoveState
{
	GENERATED_BODY()

public:
	UCharacterWalkState();

protected:
	virtual void StateTick_Implementation(float DeltaTime) override;
};
