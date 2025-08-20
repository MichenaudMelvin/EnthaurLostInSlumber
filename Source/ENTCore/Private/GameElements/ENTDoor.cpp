// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTDoor.h"
#include "AkComponent.h"

AENTDoor::AENTDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Door Mesh"));
	SetRootComponent(Root);

	MeshDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	MeshDoor->SetupAttachment(Root);

	NerveDoorNoises = CreateDefaultSubobject<UAkComponent>(TEXT("NerveDoorNoises"));
	NerveDoorNoises->SetupAttachment(Root);
}

void AENTDoor::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat UpdateEvent;
	UpdateEvent.BindDynamic(this, &AENTDoor::DoorTimelineUpdate);
	DoorTimeline.AddInterpFloat(DoorCurve, UpdateEvent);
}

void AENTDoor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	DoorMaterial = MeshDoor->CreateDynamicMaterialInstance(0, MeshDoor->GetMaterial(0));

	if (IsActiveAtStart)
	{
		DoorMaterial->SetScalarParameterValue(DoorMaterialParam, -1.0f);
		MeshDoor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		DoorMaterial->SetScalarParameterValue(DoorMaterialParam, 1.0f);
		MeshDoor->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void AENTDoor::DoorTimelineUpdate(float Alpha)
{
	float ScalarValue = FMath::Lerp(-1.0f, 1.0f, Alpha);
	DoorMaterial->SetScalarParameterValue(DoorMaterialParam, ScalarValue);
}

void AENTDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DoorTimeline.TickTimeline(DeltaTime);
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
		MeshDoor->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		DoorTimeline.SetPlayRate(1 / OpenDuration);
		DoorTimeline.Play();
	}
	else
	{
		MeshDoor->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DoorTimeline.SetPlayRate(1 / CloseDuration);
		DoorTimeline.Reverse();
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
