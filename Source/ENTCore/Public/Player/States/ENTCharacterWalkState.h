// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTCharacterMoveState.h"
#include "ENTCharacterWalkState.generated.h"

UCLASS()
class ENTCORE_API UENTCharacterWalkState : public UENTCharacterMoveState
{
	GENERATED_BODY()

public:
	UENTCharacterWalkState();

protected:
	virtual void StateTick_Implementation(float DeltaTime) override;
};
