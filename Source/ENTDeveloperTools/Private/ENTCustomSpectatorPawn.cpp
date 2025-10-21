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

	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomSpectatorPawn_RebindMovement", EKeys::AnyKey));

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

	PlayerInputComponent->ClearActionBindings();

	InputComponent->BindAxis("CustomSpectatorPawn_Turn", this, &ADefaultPawn::AddControllerYawInput);
	InputComponent->BindAxis("CustomSpectatorPawn_LookUp", this, &ADefaultPawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("CustomSpectatorPawn_RebindMovement", IE_Pressed, this, &AENTCustomSpectatorPawn::RebindMovement);

	PlayerInputComponent->ClearBindingValues();
}

void AENTCustomSpectatorPawn::RebindMovement()
{
	if (!InputComponent)
	{
		return;
	}

	InputComponent->RemoveActionBinding("CustomSpectatorPawn_RebindMovement", IE_Pressed);

	InputComponent->BindAxis("CustomSpectatorPawn_MoveForward", this, &ADefaultPawn::MoveForward);
	InputComponent->BindAxis("CustomSpectatorPawn_MoveRight", this, &ADefaultPawn::MoveRight);
}

