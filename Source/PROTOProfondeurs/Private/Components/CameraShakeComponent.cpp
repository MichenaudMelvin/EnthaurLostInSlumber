// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CameraShakeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Saves/SettingsSave.h"
#include "Saves/SettingsSubsystem.h"

UCameraShakeComponent::UCameraShakeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UCameraShakeComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner())
	{
		return;
	}

	Player = Cast<AFirstPersonCharacter>(GetOwner());
	PlayerController = Cast<AFirstPersonController>(UGameplayStatics::GetPlayerController(this, 0));

	PlayerController->ClientStartCameraShake(WalkingWobbling);
}


// Called every frame
void UCameraShakeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner() || !GetOwner()->GetGameInstance() || !Player || !PlayerController)
	{
		return;
	}

	const USettingsSubsystem* SettingsSubsystem = GetOwner()->GetGameInstance()->GetSubsystem<USettingsSubsystem>();
	if(!SettingsSubsystem)
	{
		return;
	}

	if (!SettingsSubsystem->GetSettings()->bViewBobbing)
	{
		PlayerController->ClientStopCameraShake(WalkingWobbling);
		PlayerController->ClientStopCameraShake(RunningWobbling);
		return;
	}

	if (Player->GetVelocity().Length() > 100.f && !b_IsRunning)
	{
		b_IsRunning = !b_IsRunning;

		PlayerController->ClientStopCameraShake(WalkingWobbling);
		PlayerController->ClientStartCameraShake(RunningWobbling);
	}
	else if (Player->GetVelocity().Length() <= 100.f && b_IsRunning)
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

void UCameraShakeComponent::MakeBigCameraShake()
{
	PlayerController->ClientStartCameraShake(BigShake);
}

