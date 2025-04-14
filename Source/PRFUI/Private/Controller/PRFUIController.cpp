// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PRFUIController.h"


// Sets default values
APRFUIController::APRFUIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APRFUIController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APRFUIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

