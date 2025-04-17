// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CameraShakeComponent.h"

#include <string>

#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"


UCameraShakeComponent::UCameraShakeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

// Called when the game starts
void UCameraShakeComponent::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<AFirstPersonCharacter>(GetOwner());
	PlayerController = Cast<AFirstPersonController>(UGameplayStatics::GetPlayerController(this, 0));

	PlayerController->ClientStartCameraShake(WalkingWobbling);
}


// Called every frame
void UCameraShakeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Player->GetVelocity().Length() > 100.f && !b_IsRunning)
	{
		b_IsRunning = !b_IsRunning;

		PlayerController->ClientStopCameraShake(WalkingWobbling);
		PlayerController->ClientStartCameraShake(RunningWobbling);
	} else if (Player->GetVelocity().Length() <= 100.f && b_IsRunning)
	{
		b_IsRunning = !b_IsRunning;

		PlayerController->ClientStopCameraShake(RunningWobbling);
		PlayerController->ClientStartCameraShake(WalkingWobbling);
	}
}

void UCameraShakeComponent::MakeSmallCameraShake()
{
	PlayerController->ClientStartCameraShake(LittleShake);
}

