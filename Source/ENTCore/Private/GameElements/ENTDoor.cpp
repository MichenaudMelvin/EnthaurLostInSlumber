// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTDoor.h"
#include "AkComponent.h"
#include "FCTween.h"

// Sets default values
AENTDoor::AENTDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Door Mesh"));
	SetRootComponent(Root);

	MeshDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	MeshDoor->SetupAttachment(Root);

	NerveDoorNoises = CreateDefaultSubobject<UAkComponent>(TEXT("NerveDoorNoises"));
	NerveDoorNoises->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AENTDoor::BeginPlay()
{
	Super::BeginPlay();

	DoorMaterial = MeshDoor->CreateDynamicMaterialInstance(0, MeshDoor->GetMaterial(0));

	if (IsActiveAtStart)
	{
		DoorMaterial->SetScalarParameterValue("State", -1.f);
		MeshDoor->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

// Called every frame
void AENTDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AENTDoor::Trigger_Implementation()
{
	IENTActivation::Trigger_Implementation();

	if (bIsLocked)
	{
		bIsOpened = !bIsOpened;
		return;
	}

	NerveDoorNoises->PostAssociatedAkEvent(0, FOnAkPostEventCallback());

	if (bIsOpened)
	{
		MeshDoor->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);

		FCTween::Play(
			-1.f,
			1.f,
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
			1.f,
			-1.f,
			[&](float x)
			{
				DoorMaterial->SetScalarParameterValue("State", x);
			},
			1.5f,
			EFCEase::OutSine);
	}

	bIsOpened = !bIsOpened;
}

void AENTDoor::SetLock_Implementation(bool bState)
{
	IENTActivation::SetLock_Implementation(bState);

	if (bState)
	{
		if (bIsOpened != IsActiveAtStart)
		{
			Trigger_Implementation();
			bIsLocked = true;
		}
	} else
	{
		if (bIsOpened != IsActiveAtStart)
		{
			bIsLocked = false;
			Trigger_Implementation();
		}
	}
}
