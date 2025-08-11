// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTAmberOre.h"
#include "AkComponent.h"
#include "Components/BoxComponent.h"
#include "ENTInteractableComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/States/ENTCharacterStateMachine.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTWorldSave.h"

AENTAmberOre::AENTAmberOre()
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

	Interactable = CreateDefaultSubobject<UENTInteractableComponent>(TEXT("Interactable"));
	Interactable->SetInteractionName(NSLOCTEXT("Actions", "PickAmber", "Pick Amber"));

	AmberOreNoises = CreateDefaultSubobject<UAkComponent>(TEXT("AmberOreNoises"));
	AmberOreNoises->SetupAttachment(Mesh);
}

void AENTAmberOre::BeginPlay()
{
	Super::BeginPlay();

	if (OreAmount > 0)
	{
		Interactable->AddInteractable(MeshInteraction);
		Interactable->OnInteract.AddDynamic(this, &AENTAmberOre::OnInteract);
	}
}

void AENTAmberOre::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Mesh->SetStaticMesh(SourceMesh);
}

void AENTAmberOre::Tick(float DeltaSeconds)
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

void AENTAmberOre::OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent)
{
	if (OreAmount == 0)
	{
		return;
	}

	if (!Pawn)
	{
		return;
	}

	AENTDefaultCharacter* Character = Cast<AENTDefaultCharacter>(Pawn);
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
		Character->GetStateMachine()->ChangeState(EENTCharacterStateID::Anim);
	}

	AmberOreNoises->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
	Character->MineAmber(AmberType, OreAmount);
	OreAmount--;
	OreAmount = FMath::Clamp(OreAmount, 0, 255);

	if (OreAmount == 0)
	{
		Interactable->RemoveInteractable(MeshInteraction);
		Interactable->OnInteract.RemoveDynamic(this, &AENTAmberOre::OnInteract);
	}
}

FENTGameElementData& AENTAmberOre::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	FENTAmberOreData Data;
	Data.CurrentOreAmount = OreAmount;

	return CurrentWorldSave->AmberOreData.Add(GetName(), Data);
}

void AENTAmberOre::LoadGameElement(const FENTGameElementData& GameElementData)
{
	const FENTAmberOreData& Data = static_cast<const FENTAmberOreData&>(GameElementData);
	OreAmount = Data.CurrentOreAmount;
}
