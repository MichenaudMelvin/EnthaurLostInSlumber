// Fill out your copyright notice in the Description page of Project Settings.


#include "PROTOProfondeurs/Public/GameElements/AmberOre.h"
#include "AkComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InteractableComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/States/CharacterStateMachine.h"
#include "Saves/WorldSaves/WorldSave.h"

AAmberOre::AAmberOre()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Static);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetMobility(EComponentMobility::Static);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	MeshInteraction = CreateDefaultSubobject<UBoxComponent>(TEXT("MeshInteraction"));
	MeshInteraction->SetupAttachment(Mesh);
	MeshInteraction->SetMobility(EComponentMobility::Static);
	MeshInteraction->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	AmberMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmberMesh"));
	AmberMesh->SetupAttachment(Mesh);
	AmberMesh->SetMobility(EComponentMobility::Movable);
	AmberMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Interactable = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	Interactable->SetInteractionName(NSLOCTEXT("Actions", "PickAmber", "Pick Amber"));

	AmberOreNoises = CreateDefaultSubobject<UAkComponent>(TEXT("AmberOreNoises"));
	AmberOreNoises->SetupAttachment(Mesh);
}

void AAmberOre::BeginPlay()
{
	Super::BeginPlay();

	if (OreAmount > 0)
	{
		Interactable->AddInteractable(MeshInteraction);
		Interactable->OnInteract.AddDynamic(this, &AAmberOre::OnInteract);
	}
}

void AAmberOre::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Mesh->SetStaticMesh(SourceMesh);
}

void AAmberOre::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (OreAmount != 0)
	{
		return;
	}

	FVector CurrentLocation = AmberMesh->GetRelativeLocation();
	FVector TargetLocation = FVector(0.0f, 0.0f, TargetAmberHeight);
	float Alpha = DeltaSeconds * AmberAnimSpeed;

	FVector ResultLocation = UKismetMathLibrary::VLerp(CurrentLocation, TargetLocation, Alpha);
	AmberMesh->SetRelativeLocation(ResultLocation);
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

	if (Character->GetStateMachine())
	{
		// play the spike animation
		Character->GetStateMachine()->ChangeState(ECharacterStateID::Anim);
	}

	AmberOreNoises->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
	Character->MineAmber(AmberType, OreAmount);
	OreAmount--;
	OreAmount = FMath::Clamp(OreAmount, 0, 255);

	if (OreAmount == 0)
	{
		Interactable->RemoveInteractable(MeshInteraction);
		Interactable->OnInteract.RemoveDynamic(this, &AAmberOre::OnInteract);
	}
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
}
