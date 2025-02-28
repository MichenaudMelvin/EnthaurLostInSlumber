// Fill out your copyright notice in the Description page of Project Settings.


#include "Kevin/NerveInteractable/NerveDoor.h"
#include "FCTween.h"


// Sets default values
ANerveDoor::ANerveDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ANerveDoor::BeginPlay()
{
	Super::BeginPlay();

	DoorMaterial = Mesh->CreateDynamicMaterialInstance(0, Mesh->GetMaterial(0));
}

// Called every frame
void ANerveDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANerveDoor::Activate_Implementation()
{
	INerveReactive::Activate_Implementation();

	Mesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	FCTween::Play(
		0.f,
		2.f,
		[&](float x)
		{
			DoorMaterial->SetScalarParameterValue("State", x);
		},
		1.f,
		EFCEase::InSine);
}

void ANerveDoor::Deactivate_Implementation()
{
	INerveReactive::Deactivate_Implementation();

	Mesh->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	FCTween::Play(
		2.f,
		0.f,
		[&](float x)
		{
			DoorMaterial->SetScalarParameterValue("State", x);
		},
		1.f,
		EFCEase::InSine);
}
