// Fill out your copyright notice in the Description page of Project Settings.


#include "ENTCameraShakeComponent.h"
#include "Kismet/GameplayStatics.h"

UENTCameraShakeComponent::UENTCameraShakeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UENTCameraShakeComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

void UENTCameraShakeComponent::MakeSmallCameraShake() const
{
	if (PlayerController)
	{
		PlayerController->ClientStartCameraShake(LittleShake);
	}
}

void UENTCameraShakeComponent::MakeBigCameraShake() const
{
	if (PlayerController)
	{
		PlayerController->ClientStartCameraShake(BigShake);
	}
}
