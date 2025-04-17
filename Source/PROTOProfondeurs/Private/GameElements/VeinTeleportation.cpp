// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/VeinTeleportation.h"

#include "Components/InteractableComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVeinTeleportation::AVeinTeleportation()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);

	VeinMesh = CreateDefaultSubobject<UChildActorComponent>(TEXT("Vein"));
	VeinMesh->SetupAttachment(RootComponent);

	EntryPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Entry Point"));
	EntryPoint->SetupAttachment(RootComponent);

	Interaction = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interaction"));
	Interaction->OnInteract.AddDynamic(this, &AVeinTeleportation::OnInteract);
}

// Called when the game starts or when spawned
void AVeinTeleportation::BeginPlay()
{
	Super::BeginPlay();
	Interaction->AddInteractable(EntryPoint);
}

void AVeinTeleportation::OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractComponent)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, MapToLoad);
}

