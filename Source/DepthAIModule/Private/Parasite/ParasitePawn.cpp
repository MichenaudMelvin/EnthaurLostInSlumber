// Fill out your copyright notice in the Description page of Project Settings.


#include "Parasite/ParasitePawn.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Parasite/ParasiteController.h"

AParasitePawn::AParasitePawn()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>("ParasiteHitBox");
	SetRootComponent(Box);

	Box->SetCollisionObjectType(ECC_Pawn);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Box->SetCollisionResponseToAllChannels(ECR_Block);

	ParasiteMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	ParasiteMesh->SetupAttachment(RootComponent);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("Movement");
	MovementComponent->MaxSpeed = 400.0f;

	AIControllerClass = AParasiteController::StaticClass();
}

void AParasitePawn::BeginPlay()
{
	Super::BeginPlay();
}

