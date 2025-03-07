// Fill out your copyright notice in the Description page of Project Settings.


#include "Kevin/NerveInteractable/NerveDoor.h"
#include "FCTween.h"


// Sets default values
ANerveDoor::ANerveDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Door Mesh"));
	SetRootComponent(Root);

	MeshDoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Frame"));
	MeshDoorFrame->SetupAttachment(Root);

	MeshDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	MeshDoor->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ANerveDoor::BeginPlay()
{
	Super::BeginPlay();

	DoorMaterial = MeshDoor->CreateDynamicMaterialInstance(0, MeshDoor->GetMaterial(0));

	if (IsActiveAtStart)
	{
		DoorMaterial->SetScalarParameterValue("State", 2.f);
		MeshDoor->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

// Called every frame
void ANerveDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANerveDoor::Trigger_Implementation()
{
	INerveReactive::Trigger_Implementation();

	if (IsLocked)
	{
		IsOpened = !IsOpened;
		return;
	}
	
	if (IsOpened)
	{
		MeshDoor->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		FCTween::Play(
			2.f,
			0.f,
			[&](float x)
			{
				DoorMaterial->SetScalarParameterValue("State", x);
			},
			0.5f,
			EFCEase::InSine);
	} else
	{
		MeshDoor->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		FCTween::Play(
			0.f,
			2.f,
			[&](float x)
			{
				DoorMaterial->SetScalarParameterValue("State", x);
			},
			1.5f,
			EFCEase::OutSine);
	}
	IsOpened = !IsOpened;
}

void ANerveDoor::SetLock_Implementation(bool state)
{
	INerveReactive::SetLock_Implementation(state);

	if (state)
	{
		if (IsOpened != IsActiveAtStart)
		{
			Trigger_Implementation();
			IsLocked = true;
		}
	} else
	{
		if (IsOpened != IsActiveAtStart)
		{
			IsLocked = false;
			Trigger_Implementation();
		}
	}
}
