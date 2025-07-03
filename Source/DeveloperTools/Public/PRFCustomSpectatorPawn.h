// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "PRFCustomSpectatorPawn.generated.h"

UCLASS()
class DEVELOPERTOOLS_API APRFCustomSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	APRFCustomSpectatorPawn();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
