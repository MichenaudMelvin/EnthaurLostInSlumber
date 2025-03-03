// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"
#include "CharacterTakeAmberState.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UCharacterTakeAmberState : public UCharacterState
{
	GENERATED_BODY()

public:
	UCharacterTakeAmberState();

protected:
	virtual void StateEnter_Implementation(const ECharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;
};
