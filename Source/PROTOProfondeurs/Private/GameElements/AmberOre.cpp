// Fill out your copyright notice in the Description page of Project Settings.


#include "PROTOProfondeurs/Public/GameElements/AmberOre.h"
#include "Components/InteractableComponent.h"
#include "Player/FirstPersonCharacter.h"
#include "Saves/WorldSaves/WorldSave.h"

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

	if (OreAmount > 0)
	{
		Interactable->AddInteractable(Mesh);
		Interactable->OnInteract.AddDynamic(this, &AAmberOre::OnInteract);
	}

	UpdateMaterial(OreAmount == 0);
}

void AAmberOre::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateMaterial(false);
}

void AAmberOre::OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent)
{
	if (OreAmount == 0)
	{
		return;
	}

	if (!Pawn)
	{
		return;
	}

	AFirstPersonCharacter* Character = Cast<AFirstPersonCharacter>(Pawn);
	if (!Character)
	{
		return;
	}

	if (Character->IsAmberTypeFilled(AmberType))
	{
		return;
	}

	Character->MineAmber(AmberType, OreAmount);
	OreAmount--;
	OreAmount = FMath::Clamp(OreAmount, 0, 255);

	if (OreAmount == 0)
	{
		UpdateMaterial(OreAmount == 0);

		Interactable->RemoveInteractable(Mesh);
		Interactable->OnInteract.RemoveDynamic(this, &AAmberOre::OnInteract);
	}
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

FGameElementData& AAmberOre::SaveGameElement(UWorldSave* CurrentWorldSave)
{
	FAmberOreData Data;
	Data.CurrentOreAmount = OreAmount;

	return CurrentWorldSave->AmberOreData.Add(GetName(), Data);
}

void AAmberOre::LoadGameElement(const FGameElementData& GameElementData)
{
	const FAmberOreData& Data = static_cast<const FAmberOreData&>(GameElementData);
	OreAmount = Data.CurrentOreAmount;

	// material update done by the BeginPlay()
}
