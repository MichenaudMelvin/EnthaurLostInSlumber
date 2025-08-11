// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTTeleportationVein.h"
#include "ENTInteractableComponent.h"

AENTTeleportationVein::AENTTeleportationVein()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);

	Interaction = CreateDefaultSubobject<UENTInteractableComponent>(TEXT("Interaction"));
	Interaction->OnInteract.AddDynamic(this, &AENTTeleportationVein::OnInteract);
}

void AENTTeleportationVein::OnEnterWeakZone_Implementation(bool bIsZoneActive)
{
	IENTWeakZoneInterface::OnEnterWeakZone_Implementation(bIsZoneActive);

	if (bIsZoneActive && Interaction->OnInteract.IsAlreadyBound(this, &AENTTeleportationVein::OnInteract))
	{
		Interaction->OnInteract.RemoveDynamic(this, &AENTTeleportationVein::OnInteract);
	}
}

void AENTTeleportationVein::OnExitWeakZone_Implementation()
{
	IENTWeakZoneInterface::OnExitWeakZone_Implementation();

	if (!Interaction->OnInteract.IsAlreadyBound(this, &AENTTeleportationVein::OnInteract))
	{
		Interaction->OnInteract.AddDynamic(this, &AENTTeleportationVein::OnInteract);
	}
}

void AENTTeleportationVein::OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractComponent)
{
	OnVeinInteracted.Broadcast();
}

