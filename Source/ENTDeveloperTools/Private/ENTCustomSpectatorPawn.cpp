// Fill out your copyright notice in the Description page of Project Settings.


#include "ENTCustomSpectatorPawn.h"
#include "GameFramework/PlayerInput.h"

void InitializeCustomSpectatorPawnInputBindings()
{
	static bool bBindingsAdded = false;
	if (bBindingsAdded)
	{
		return;
	}

	bBindingsAdded = true;

	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CustomSpectatorPawn_MoveForward", EKeys::Z, 1.0f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CustomSpectatorPawn_MoveRight", EKeys::Q, -1.0f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CustomSpectatorPawn_MoveForward", EKeys::S, -1.0f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CustomSpectatorPawn_MoveRight", EKeys::D, 1.0f));

	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CustomSpectatorPawn_Turn", EKeys::MouseX, 1.0f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CustomSpectatorPawn_LookUp", EKeys::MouseY, -1.0f));
}

AENTCustomSpectatorPawn::AENTCustomSpectatorPawn()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AENTCustomSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Override the default pawn inputs
	// Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!bAddDefaultMovementBindings)
	{
		return;
	}

	InitializeCustomSpectatorPawnInputBindings();

	PlayerInputComponent->BindAxis("CustomSpectatorPawn_MoveForward", this, &ADefaultPawn::MoveForward);
	PlayerInputComponent->BindAxis("CustomSpectatorPawn_MoveRight", this, &ADefaultPawn::MoveRight);
	PlayerInputComponent->BindAxis("CustomSpectatorPawn_Turn", this, &ADefaultPawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("CustomSpectatorPawn_LookUp", this, &ADefaultPawn::AddControllerPitchInput);
}

