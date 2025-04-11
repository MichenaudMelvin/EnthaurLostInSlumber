// Fill out your copyright notice in the Description page of Project Settings.


#include "PROTOProfondeurs/Public/GameElements/AmberOre.h"
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

void AAmberOre::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateMaterial(false);
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

	if (Character->IsAmberTypeFilled(AmberType))
	{
		return;
	}

	Character->MineAmber(AmberType, OreAmount);
	Interactable->RemoveInteractable(Mesh);

	UpdateMaterial(true);

	Interactable->OnInteract.RemoveDynamic(this, &AAmberOre::OnInteract);
}

void AAmberOre::UpdateMaterial(bool bPickedUp) const
{
	TObjectPtr<UMaterialInterface> TargetMaterial = nullptr;
	switch (AmberType)
	{
	case EAmberType::NecroseAmber:
		TargetMaterial = bPickedUp ? NecroseMaterialPickUp : NecroseMaterial;
		break;
	case EAmberType::WeakAmber:
		TargetMaterial = bPickedUp ? WeakMaterialPickUp : WeakMaterial;
		break;
	}

	Mesh->SetMaterial(0, TargetMaterial);
}
