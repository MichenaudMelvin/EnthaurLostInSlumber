// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FirstPersonSpectator.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

AFirstPersonSpectator::AFirstPersonSpectator()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	Movement->UpdatedComponent = RootComponent;
	Movement->MaxSpeed = 2500.0f;
	Movement->Deceleration = INFINITY;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Root);

#if WITH_EDITORONLY_DATA
	Camera->bCameraMeshHiddenInGame = false;
#endif
}

void AFirstPersonSpectator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PlayerController = Cast<APlayerController>(Controller);

	if (!PlayerController)
	{
		return;
	}

	if (PlayerController->IsInputKeyDown(EKeys::Z))
	{
		AddMovementInput(GetActorForwardVector(), 1.0f, true);
	}

	else if (PlayerController->IsInputKeyDown(EKeys::S))
	{
		AddMovementInput(GetActorForwardVector() * -1, 1.0f, true);
	}

	if (PlayerController->IsInputKeyDown(EKeys::Q))
	{
		AddMovementInput(GetActorRightVector() * -1, 1.0f, true);
	}

	else if (PlayerController->IsInputKeyDown(EKeys::D))
	{
		AddMovementInput(GetActorRightVector(), 1.0f, true);
	}
}

void AFirstPersonSpectator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxisKey(EKeys::MouseY, this, &AFirstPersonSpectator::RotatePitch);
	PlayerInputComponent->BindAxisKey(EKeys::MouseX, this, &AFirstPersonSpectator::RotateYaw);
}

void AFirstPersonSpectator::RotatePitch(float AxisValue)
{
	AddControllerPitchInput(AxisValue * CameraSpeed * -1);
}

void AFirstPersonSpectator::RotateYaw(float AxisValue)
{
	AddControllerYawInput(AxisValue * CameraSpeed);
}


