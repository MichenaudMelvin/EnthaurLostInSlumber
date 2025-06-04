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

	Interaction = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interaction"));
	Interaction->OnInteract.AddDynamic(this, &AVeinTeleportation::OnInteract);
}

void AVeinTeleportation::OnEnterWeakZone_Implementation(bool bIsZoneActive)
{
	IWeakZoneInterface::OnEnterWeakZone_Implementation(bIsZoneActive);

	if (bIsZoneActive && Interaction->OnInteract.IsAlreadyBound(this, &AVeinTeleportation::OnInteract))
	{
		Interaction->OnInteract.RemoveDynamic(this, &AVeinTeleportation::OnInteract);
	}
}

void AVeinTeleportation::OnExitWeakZone_Implementation()
{
	IWeakZoneInterface::OnExitWeakZone_Implementation();

	if (!Interaction->OnInteract.IsAlreadyBound(this, &AVeinTeleportation::OnInteract))
	{
		Interaction->OnInteract.AddDynamic(this, &AVeinTeleportation::OnInteract);
	}
}

void AVeinTeleportation::OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractComponent)
{
	OnVeinInteracted.Broadcast();
}

