// Fill out your copyright notice in the Description page of Project Settings.


#include "Gamefeel/ENTElectricityFeedback.h"
#include "AkComponent.h"
#include "Components/PostProcessComponent.h"


AENTElectricityFeedback::AENTElectricityFeedback()
{
	PrimaryActorTick.bCanEverTick = true;
	Radius = 50.0f;

	Electricity = CreateDefaultSubobject<UPostProcessComponent>(TEXT("Electricity"));
	SetRootComponent(Electricity);

	ElectricityNoises = CreateDefaultSubobject<UAkComponent>(TEXT("ElectricityNoises"));
	ElectricityNoises->SetupAttachment(Electricity);
}

void AENTElectricityFeedback::BeginPlay()
{
	Super::BeginPlay();

	FOnAkPostEventCallback CallBackEvent;
	CallBackEvent.BindDynamic(this, &AENTElectricityFeedback::PlayNoise);

	// would prefer an infinite sound
	ElectricityNoises->PostAssociatedAkEvent(static_cast<int32>(EAkCallbackType::EndOfEvent), CallBackEvent);
}

void AENTElectricityFeedback::PlayNoise(EAkCallbackType CallbackType, UAkCallbackInfo* CallbackInfo)
{
	if (CallbackType != EAkCallbackType::EndOfEvent)
	{
		return;
	}

	FOnAkPostEventCallback CallBackEvent;
	CallBackEvent.BindDynamic(this, &AENTElectricityFeedback::PlayNoise);
	ElectricityNoises->PostAssociatedAkEvent(static_cast<int32>(EAkCallbackType::EndOfEvent), CallBackEvent);
}
