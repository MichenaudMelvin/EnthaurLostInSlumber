// Fill out your copyright notice in the Description page of Project Settings.


#include "PRFCustomDebugCamera.h"
#include "GameFramework/DefaultPawn.h"
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
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomDebugCamera_Teleport", EKeys::RightMouseButton));
}

void APRFCustomDebugCamera::SetupInputComponent()
{
	Super::SetupInputComponent();

	InitializeDebugCameraInputBindings();
	InputComponent->BindAction("CustomDebugCamera_Teleport", IE_Pressed, this, &APRFCustomDebugCamera::Teleport);
}

void APRFCustomDebugCamera::Teleport()
{
	if (!OriginalControllerRef && !OriginalControllerRef->GetPawn())
	{
		return;
	}

	FVector CamLocation;
	FRotator CamRotation;
	GetPlayerViewPoint(CamLocation, CamRotation);

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(NAME_None, FCollisionQueryParams::GetUnknownStatId(), true, OriginalControllerRef->GetPawn());
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CamLocation, CamLocation + (CamRotation.Vector() * TeleportTraceLength), ECC_Pawn, TraceParams);
	if(!bHit)
	{
		return;
	}

	OriginalControllerRef->GetPawn()->SetActorLocation(HitResult.Location, false);
	UKismetSystemLibrary::ExecuteConsoleCommand(this, "ToggleDebugCamera", this);
}
