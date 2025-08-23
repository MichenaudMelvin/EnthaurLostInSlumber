// Fill out your copyright notice in the Description page of Project Settings.

#include "ENTCustomDebugCamera.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void InitializeCustomDebugCameraInputBindings()
{
	static bool bBindingsAdded = false;
	if (bBindingsAdded)
	{
		return;
	}

	bBindingsAdded = true;
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomDebugCamera_ShowTeleportLocation", EKeys::RightMouseButton));
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

	bShowTeleportLocation = false;
	ToggleDisplay();
}

void AENTCustomDebugCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bShowTeleportLocation)
	{
		ShowTeleportLocation();
	}
}

void AENTCustomDebugCamera::SetupInputComponent()
{
	Super::SetupInputComponent();

	InitializeCustomDebugCameraInputBindings();
	InputComponent->BindAction("CustomDebugCamera_ShowTeleportLocation", IE_Pressed, this, &AENTCustomDebugCamera::TriggerShowTeleportLocation);
	InputComponent->BindAction("CustomDebugCamera_TeleportToFacingLocation", IE_Released, this, &AENTCustomDebugCamera::TeleportToFacingLocation);
	InputComponent->BindAction("CustomDebugCamera_DestroyFacingActor", IE_Pressed, this, &AENTCustomDebugCamera::DestroyFacingActor);

	InputComponent->RemoveActionBinding("DebugCamera_IncreaseSpeed", IE_Pressed);
	InputComponent->RemoveActionBinding("DebugCamera_DecreaseSpeed", IE_Pressed);

	InputComponent->BindAction("CustomDebugCamera_IncreaseSpeed", IE_Pressed, this, &AENTCustomDebugCamera::CustomIncreaseCameraSpeed);
	InputComponent->BindAction("CustomDebugCamera_DecreaseSpeed", IE_Pressed, this, &AENTCustomDebugCamera::CustomDecreaseCameraSpeed);
}

bool AENTCustomDebugCamera::Trace(FHitResult& HitResult) const
{
	FVector CamLocation;
	FRotator CamRotation;
	GetPlayerViewPoint(CamLocation, CamRotation);

	FCollisionQueryParams TraceParams(NAME_None, FCollisionQueryParams::GetUnknownStatId(), false);

	if (OriginalControllerRef && OriginalControllerRef->GetPawn())
	{
		TraceParams.AddIgnoredActor(OriginalControllerRef->GetPawn());
	}

	return GetWorld()->LineTraceSingleByChannel(HitResult, CamLocation, CamLocation + (CamRotation.Vector() * TraceLength), ECC_Pawn, TraceParams);
}

bool AENTCustomDebugCamera::GetTeleportationResult(FVector& TeleportLocation, FRotator& TeleportControlRotation) const
{
	if (!OriginalControllerRef || !OriginalControllerRef->GetPawn())
	{
		const FString Message = FString::Printf(TEXT("Controller or pawn doesn't exist"));

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
		return false;
	}

	FHitResult HitResult;
	if(!Trace(HitResult))
	{
		return false;
	}

	ACharacter* CharacterObject = Cast<ACharacter>(OriginalControllerRef->GetPawn());

	if (CharacterObject)
	{
		float CapsuleHalfHeight = CharacterObject->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		HitResult.Location.Z += CapsuleHalfHeight * (HitResult.Normal.Z >= 0.0f ? 1.0f : -1.0f);
	}

	TeleportLocation = HitResult.Location;

	TeleportControlRotation = GetControlRotation();

	if (bResetCameraPitch)
	{
		TeleportControlRotation.Pitch = 0.0f;
	}

	return true;
}

void AENTCustomDebugCamera::TriggerShowTeleportLocation()
{
	bShowTeleportLocation = true;
}

void AENTCustomDebugCamera::ShowTeleportLocation() const
{
	FVector Location;
	FRotator Rotation;
	if (!GetTeleportationResult(Location, Rotation))
	{
		return;
	}

	ACharacter* CharacterObject = Cast<ACharacter>(OriginalControllerRef->GetPawn());

	if (CharacterObject)
	{
		float HalfHeight = CharacterObject->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		float Radius = CharacterObject->GetCapsuleComponent()->GetScaledCapsuleRadius();

		UKismetSystemLibrary::DrawDebugCapsule(this, Location, HalfHeight, Radius, FRotator::ZeroRotator, FLinearColor::Red, 0.0f, 1.0f);
	}

	UKismetSystemLibrary::DrawDebugPoint(this, Location, 5.0f, FLinearColor::Green, 0.0f);

	FVector Direction = UKismetMathLibrary::GetForwardVector(Rotation);
	FVector EndLocation = Location + (Direction * ArrowLength);
	UKismetSystemLibrary::DrawDebugArrow(this, Location, EndLocation, 5.0f, FLinearColor::Red, 0, 1.0f);
}

void AENTCustomDebugCamera::TeleportToFacingLocation()
{
	bShowTeleportLocation = false;

	FVector Location;
	FRotator Rotation;
	if (!GetTeleportationResult(Location, Rotation))
	{
		return;
	}

	OriginalControllerRef->GetPawn()->SetActorLocation(Location, false);
	OriginalControllerRef->SetControlRotation(Rotation);
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
