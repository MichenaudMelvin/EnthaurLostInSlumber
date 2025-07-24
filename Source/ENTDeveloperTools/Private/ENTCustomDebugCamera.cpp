// Fill out your copyright notice in the Description page of Project Settings.

#include "ENTCustomDebugCamera.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/KismetSystemLibrary.h"

void InitializeCustomDebugCameraInputBindings()
{
	static bool bBindingsAdded = false;
	if (bBindingsAdded)
	{
		return;
	}

	bBindingsAdded = true;
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomDebugCamera_TeleportToFacingLocation", EKeys::RightMouseButton));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomDebugCamera_DestroyFacingActor", EKeys::Delete));

	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomDebugCamera_IncreaseSpeed", EKeys::Add));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomDebugCamera_IncreaseSpeed", EKeys::MouseScrollUp));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomDebugCamera_IncreaseSpeed", EKeys::Gamepad_RightShoulder));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomDebugCamera_DecreaseSpeed", EKeys::Subtract));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomDebugCamera_DecreaseSpeed", EKeys::MouseScrollDown));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomDebugCamera_DecreaseSpeed", EKeys::Gamepad_LeftShoulder));
}

AENTCustomDebugCamera::AENTCustomDebugCamera()
{
	InitialMaxSpeed = 2.0f;
	SpeedScale = 2.0f;
}

void AENTCustomDebugCamera::BeginPlay()
{
	Super::BeginPlay();

	ToggleDisplay();
}

void AENTCustomDebugCamera::SetupInputComponent()
{
	Super::SetupInputComponent();

	InitializeCustomDebugCameraInputBindings();
	InputComponent->BindAction("CustomDebugCamera_TeleportToFacingLocation", IE_Pressed, this, &AENTCustomDebugCamera::TeleportToFacingLocation);
	InputComponent->BindAction("CustomDebugCamera_DestroyFacingActor", IE_Pressed, this, &AENTCustomDebugCamera::DestroyFacingActor);

	InputComponent->RemoveActionBinding("DebugCamera_IncreaseSpeed", IE_Pressed);
	InputComponent->RemoveActionBinding("DebugCamera_DecreaseSpeed", IE_Pressed);

	InputComponent->BindAction("CustomDebugCamera_IncreaseSpeed", IE_Pressed, this, &AENTCustomDebugCamera::CustomIncreaseCameraSpeed);
	InputComponent->BindAction("CustomDebugCamera_DecreaseSpeed", IE_Pressed, this, &AENTCustomDebugCamera::CustomDecreaseCameraSpeed);
}

bool AENTCustomDebugCamera::Trace(FHitResult& HitResult) const
{
	if (!OriginalControllerRef && !OriginalControllerRef->GetPawn())
	{
		return false;
	}

	FVector CamLocation;
	FRotator CamRotation;
	GetPlayerViewPoint(CamLocation, CamRotation);

	FCollisionQueryParams TraceParams(NAME_None, FCollisionQueryParams::GetUnknownStatId(), true, OriginalControllerRef->GetPawn());
	return GetWorld()->LineTraceSingleByChannel(HitResult, CamLocation, CamLocation + (CamRotation.Vector() * TraceLength), ECC_Pawn, TraceParams);
}

void AENTCustomDebugCamera::TeleportToFacingLocation()
{
	FHitResult HitResult;
	if(!Trace(HitResult) && !OriginalControllerRef && !OriginalControllerRef->GetPawn())
	{
		return;
	}

	OriginalControllerRef->GetPawn()->SetActorLocation(HitResult.Location, false);
	UKismetSystemLibrary::ExecuteConsoleCommand(this, "ToggleDebugCamera", this);
}

void AENTCustomDebugCamera::DestroyFacingActor()
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

void AENTCustomDebugCamera::CustomIncreaseCameraSpeed()
{
	SpeedScale += CameraSpeedScaleStep;
	ApplySpeedScale();
}

void AENTCustomDebugCamera::CustomDecreaseCameraSpeed()
{
	SpeedScale -= CameraSpeedScaleStep;
	SpeedScale = FMath::Max(SpeedScale, CameraSpeedScaleStep);
	ApplySpeedScale();
}
