// Fill out your copyright notice in the Description page of Project Settings.


#include "Environement/AmberOre.h"
#include "Components/InteractableComponent.h"
#include "Player/FirstPersonCharacter.h"


AAmberOre::AAmberOre()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	Interactable = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	Interactable->SetInteractionName(NSLOCTEXT("Actions", "PickAmber", "Pick Amber"));
}

void AAmberOre::BeginPlay()
{
	Super::BeginPlay();

	Interactable->AddInteractable(Mesh);
	Interactable->OnInteract.AddDynamic(this, &AAmberOre::OnInteract);
}

void AAmberOre::OnInteract(APlayerController* Controller, APawn* Pawn)
{
	if (Pawn == nullptr)
	{
		return;
	}

	AFirstPersonCharacter* Character = Cast<AFirstPersonCharacter>(Pawn);
	if (Character == nullptr)
	{
		return;
	}

	if (Character->IsAmberFilled())
	{
		return;
	}

	Character->FillAmber(true);
	Interactable->RemoveInteractable(Mesh);
	Interactable->OnInteract.RemoveDynamic(this, &AAmberOre::OnInteract);
}
