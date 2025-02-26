// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterMoveState.h"
#include "CharacterSlideState.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UCharacterSlideState : public UCharacterMoveState
{
	GENERATED_BODY()

public:
	UCharacterSlideState();

protected:
	virtual void StateTick_Implementation(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sliding", meta = (UIMin = 0.0f, UIMax = 1.0f, ClampMin = 0.0f, ClampMax = 1.0f))
	float SlipperyFactor = 1.0f;

	virtual void OnWalkOnNewSurface_Implementation(const TEnumAsByte<EPhysicalSurface>& NewSurface) override;
};
