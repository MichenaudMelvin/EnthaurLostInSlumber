// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/RespawnTree.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "FCTween.h"
#include "Components/BoxComponent.h"
#include "Components/InteractableComponent.h"
#include "GameModes/FirstPersonGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"
#include "Saves/WorldSaves/WorldSave.h"
#include "Saves/WorldSaves/WorldSaveSubsystem.h"

// Sets default values
ARespawnTree::ARespawnTree()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	TreeModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreeModel"));
	TreeModel->SetupAttachment(RootComp);
	TreeModel->SetMobility(EComponentMobility::Static);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(TreeModel);
	TriggerBox->SetMobility(EComponentMobility::Static);

	RespawnTreeNoises = CreateDefaultSubobject<UAkComponent>(TEXT("RespawnTreeNoises"));
	RespawnTreeNoises->SetupAttachment(RootComp);

	Interaction = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interaction"));
}

// Called when the game starts or when spawned
void ARespawnTree::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ARespawnTree::TriggerEnter);

	RespawnTransform *= GetActorTransform();

	Material = TreeModel->CreateDynamicMaterialInstance(0, TreeModel->GetMaterial(0));
	if (bIsActivated)
	{
		SetActive();

		if (LastCheckPointName != GetName())
		{
			return;
		}

		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (!Character)
		{
			return;
		}

		AFirstPersonCharacter* Player = Cast<AFirstPersonCharacter>(Character);
		if (!Player)
		{
			return;
		}

		SetRespawnPoint(Player, false);
	}
	else
	{
		TreeModel->SetMaterial(0, Material);
		Material->SetScalarParameterValue("Emissive", 0.f);
		Interaction->AddInteractable(TreeModel);
		Interaction->OnInteract.AddDynamic(this, &ARespawnTree::Interact);
	}
}

void ARespawnTree::Destroyed()
{
	Super::Destroyed();

	if (Interaction && Interaction->OnInteract.IsAlreadyBound(this, &ARespawnTree::Interact))
	{
		Interaction->OnInteract.RemoveDynamic(this, &ARespawnTree::Interact);
	}

	if (TriggerBox && TriggerBox->OnComponentBeginOverlap.IsAlreadyBound(this, &ARespawnTree::TriggerEnter))
	{
		TriggerBox->OnComponentBeginOverlap.RemoveDynamic(this, &ARespawnTree::TriggerEnter);
	}
}

void ARespawnTree::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector RelativeLocation = TriggerBox->GetRelativeLocation();
	TriggerBox->SetRelativeLocation(FVector(RelativeLocation.X, RelativeLocation.Y, TriggerBox->GetUnscaledBoxExtent().Z));
}

void ARespawnTree::TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFirstPersonCharacter* Player = Cast<AFirstPersonCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	if (!bIsActivated)
	{
		SetActive();
	}

	SetRespawnPoint(Player, true);
}

void ARespawnTree::Interact(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent)
{
	AFirstPersonCharacter* Player = Cast<AFirstPersonCharacter>(Pawn);
	if (!Player)
	{
		return;
	}

	if (!bIsActivated)
	{
		SetActive();
	}

	SetRespawnPoint(Player, true);
}

void ARespawnTree::SetActive()
{
	bIsActivated = true;

	RespawnTreeNoises->PostAssociatedAkEvent(0, FOnAkPostEventCallback());

	FCTween::Play(
		0.f,
		30.f,
		[&](float x)
		{
			Material->SetScalarParameterValue("Emissive", x);
		},
		2.f,
		EFCEase::InSine
	);
}

void ARespawnTree::SetRespawnPoint(AFirstPersonCharacter* Player, bool bSave)
{
	Player->SetRespawnTree(this);

	if (!bSave)
	{
		return;
	}

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
	if (!GameMode)
	{
		return;
	}

	AFirstPersonGameMode* FirstPersonGameMode = Cast<AFirstPersonGameMode>(GameMode);
	if (!FirstPersonGameMode)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Invalid GameMode, please make sure the current GameMode is %s to save the world"), *FirstPersonGameMode->GetClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	FirstPersonGameMode->SaveWorld();
}

void ARespawnTree::OnEnterWeakZone_Implementation(bool bIsZoneActive)
{
	IWeakZoneInterface::OnEnterWeakZone_Implementation(bIsZoneActive);

	if (bIsZoneActive && Interaction->OnInteract.IsAlreadyBound(this, &ARespawnTree::Interact))
	{
		Interaction->OnInteract.RemoveDynamic(this, &ARespawnTree::Interact);
	}
}

void ARespawnTree::OnExitWeakZone_Implementation()
{
	IWeakZoneInterface::OnExitWeakZone_Implementation();

	if (!Interaction->OnInteract.IsAlreadyBound(this, &ARespawnTree::Interact))
	{
		Interaction->OnInteract.AddDynamic(this, &ARespawnTree::Interact);
	}
}

FGameElementData& ARespawnTree::SaveGameElement(UWorldSave* CurrentWorldSave)
{
	FRespawnTreeData Data;
	Data.bIsActive = bIsActivated;

	return CurrentWorldSave->RespawnTreeData.Add(GetName(), Data);
}

void ARespawnTree::LoadGameElement(const FGameElementData& GameElementData)
{
	const FRespawnTreeData& Data = static_cast<const FRespawnTreeData&>(GameElementData);
	bIsActivated = Data.bIsActive;

	UWorldSaveSubsystem* WorldSaveSubsystem = GetGameInstance()->GetSubsystem<UWorldSaveSubsystem>();
	if(!WorldSaveSubsystem)
	{
		return;
	}

	LastCheckPointName = WorldSaveSubsystem->GetCurrentWorldSave()->LastCheckPointName;
}
