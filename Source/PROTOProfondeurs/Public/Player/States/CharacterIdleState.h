// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"
#include "CharacterIdleState.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UCharacterIdleState : public UCharacterState
{
	GENERATED_BODY()

public:
	UCharacterIdleState();

protected:
	virtual void StateTick_Implementation(float DeltaTime) override;
};
