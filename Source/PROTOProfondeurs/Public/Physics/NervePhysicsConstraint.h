// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NervePhysicsConstraint.generated.h"

UCLASS(Config = Game, DefaultConfig)
class PROTOPROFONDEURS_API UNervePhysicsConstraint : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Physics")
	FFloatRange CameraPropulsion = FFloatRange(0.0f, 0.7f);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
