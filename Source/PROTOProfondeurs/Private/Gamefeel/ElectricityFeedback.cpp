// Fill out your copyright notice in the Description page of Project Settings.


#include "Gamefeel/ElectricityFeedback.h"

#include "Components/PostProcessComponent.h"


// Sets default values
AElectricityFeedback::AElectricityFeedback()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Radius = 50.f;

	Electricity = CreateDefaultSubobject<UPostProcessComponent>(TEXT("Electricity"));
	SetRootComponent(Electricity);
}

// Called when the game starts or when spawned
void AElectricityFeedback::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AElectricityFeedback::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

