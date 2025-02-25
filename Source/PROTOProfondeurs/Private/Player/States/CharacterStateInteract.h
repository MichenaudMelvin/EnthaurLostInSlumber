// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/States/CharacterState.h"
#include "CharacterStateInteract.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UCharacterStateInteract : public UCharacterState
{
	GENERATED_BODY()

public:
	UCharacterStateInteract();

protected:
	virtual void StateEnter_Implementation(const ECharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interact", meta = (ClampMin = 0, Units = "s"))
	float InteractionDuration = 0.3f;

private:
	float Duration = 0.0f;
};
