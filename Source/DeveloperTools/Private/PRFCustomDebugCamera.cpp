// Fill out your copyright notice in the Description page of Project Settings.

#include "PRFCustomDebugCamera.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/KismetSystemLibrary.h"

void InitializeDebugCameraInputBindings()
{
	static bool bBindingsAdded = false;
	if (bBindingsAdded)
	{
		return;
	}

	bBindingsAdded = true;
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomDebugCamera_TeleportToFacingLocation", EKeys::RightMouseButton));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomDebugCamera_DestroyFacingActor", EKeys::BackSpace));
}

void APRFCustomDebugCamera::SetupInputComponent()
{
	Super::SetupInputComponent();

	InitializeDebugCameraInputBindings();
	InputComponent->BindAction("CustomDebugCamera_TeleportToFacingLocation", IE_Pressed, this, &APRFCustomDebugCamera::TeleportToFacingLocation);
	InputComponent->BindAction("CustomDebugCamera_DestroyFacingActor", IE_Pressed, this, &APRFCustomDebugCamera::DestroyFacingActor);
}

bool APRFCustomDebugCamera::Trace(FHitResult& HitResult) const
{
	if (!OriginalControllerRef && !OriginalControllerRef->GetPawn())
	{
		return false;
	}

	FVector CamLocation;
	FRotator CamRotation;
	GetPlayerViewPoint(CamLocation, CamRotation);

	FCollisionQueryParams TraceParams(NAME_None, FCollisionQueryParams::GetUnknownStatId(), true, OriginalControllerRef->GetPawn());
	return GetWorld()->LineTraceSingleByChannel(HitResult, CamLocation, CamLocation + (CamRotation.Vector() * TeleportTraceLength), ECC_Pawn, TraceParams);
}

void APRFCustomDebugCamera::TeleportToFacingLocation()
{
	FHitResult HitResult;
	if(!Trace(HitResult) && !OriginalControllerRef && !OriginalControllerRef->GetPawn())
	{
		return;
	}

	OriginalControllerRef->GetPawn()->SetActorLocation(HitResult.Location, false);
	UKismetSystemLibrary::ExecuteConsoleCommand(this, "ToggleDebugCamera", this);
}

void APRFCustomDebugCamera::DestroyFacingActor()
{
	FHitResult HitResult;
	if(!Trace(HitResult))
	{
		return;
	}

	if (!HitResult.GetActor())
	{
		return;
	}

	HitResult.GetActor()->Destroy();
}
