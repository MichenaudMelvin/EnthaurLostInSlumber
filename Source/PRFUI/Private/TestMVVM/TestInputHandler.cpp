// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMVVM/TestInputHandler.h"

ATestInputHandler::ATestInputHandler()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATestInputHandler::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (IsValid(PlayerController))
	{
		EnableInput(PlayerController);

		if (!InputComponent)
		{
			InputComponent = NewObject<UInputComponent>(this);
			InputComponent->RegisterComponent();
		}

		InputComponent->BindAction("IncreaseCounter", IE_Pressed, this, &ATestInputHandler::OnIncreaseCounterPressed);
	}
}

void ATestInputHandler::SetViewModel(UTestViewModel* InViewModel)
{
	ViewModel = InViewModel;
}

void ATestInputHandler::OnIncreaseCounterPressed()
{
	if (ViewModel)
	{
		ViewModel->IncreaseCounter();
	}
}
