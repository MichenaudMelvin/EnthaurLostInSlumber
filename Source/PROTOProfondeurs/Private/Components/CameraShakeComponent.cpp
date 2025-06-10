// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CameraShakeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Saves/SettingsSave.h"
#include "Saves/SettingsSubsystem.h"

UCameraShakeComponent::UCameraShakeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UCameraShakeComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<AFirstPersonController>(UGameplayStatics::GetPlayerController(this, 0));
}

void UCameraShakeComponent::MakeSmallCameraShake()
{
	PlayerController->ClientStartCameraShake(LittleShake);
}

void UCameraShakeComponent::MakeBigCameraShake()
{
	PlayerController->ClientStartCameraShake(BigShake);
}

