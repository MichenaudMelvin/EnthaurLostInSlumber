// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "ENTCustomSpectatorPawn.generated.h"

UCLASS()
class ENTDEVELOPERTOOLS_API AENTCustomSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	AENTCustomSpectatorPawn();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void RebindMovement();
};
