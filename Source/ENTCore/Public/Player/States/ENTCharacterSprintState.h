// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTCharacterMoveState.h"
#include "ENTCharacterSprintState.generated.h"

UCLASS()
class ENTCORE_API UENTCharacterSprintState : public UENTCharacterMoveState
{
	GENERATED_BODY()

public:
	UENTCharacterSprintState();

protected:
	virtual void StateTick_Implementation(float DeltaTime) override;
};
