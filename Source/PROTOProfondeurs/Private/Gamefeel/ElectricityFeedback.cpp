// Fill out your copyright notice in the Description page of Project Settings.


#include "Gamefeel/ElectricityFeedback.h"
#include "AkComponent.h"
#include "Components/PostProcessComponent.h"


// Sets default values
AElectricityFeedback::AElectricityFeedback()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Radius = 50.f;

	Electricity = CreateDefaultSubobject<UPostProcessComponent>(TEXT("Electricity"));
	SetRootComponent(Electricity);

	ElectricityNoises = CreateDefaultSubobject<UAkComponent>(TEXT("ElectricityNoises"));
	ElectricityNoises->SetupAttachment(Electricity);
}

void AElectricityFeedback::BeginPlay()
{
	Super::BeginPlay();

	FOnAkPostEventCallback CallBackEvent;
	CallBackEvent.BindDynamic(this, &AElectricityFeedback::PlayNoise);

	// would prefer an infinite sound
	ElectricityNoises->PostAssociatedAkEvent(static_cast<int32>(EAkCallbackType::EndOfEvent), CallBackEvent);
}

void AElectricityFeedback::PlayNoise(EAkCallbackType CallbackType, UAkCallbackInfo* CallbackInfo)
{
	if (CallbackType != EAkCallbackType::EndOfEvent)
	{
		return;
	}

	FOnAkPostEventCallback CallBackEvent;
	CallBackEvent.BindDynamic(this, &AElectricityFeedback::PlayNoise);
	ElectricityNoises->PostAssociatedAkEvent(static_cast<int32>(EAkCallbackType::EndOfEvent), CallBackEvent);
}
