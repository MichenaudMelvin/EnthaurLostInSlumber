// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/RespawnTree.h"
#include "AkGameplayStatics.h"
#include "FCTween.h"
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

	auto Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TreeModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreeModel"));
	TreeModel->SetupAttachment(Root);

	RespawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Respawn Point"));
	RespawnPoint->SetupAttachment(Root);

	Interaction = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interaction"));
}

// Called when the game starts or when spawned
void ARespawnTree::BeginPlay()
{
	Super::BeginPlay();

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

	Interaction->OnInteract.RemoveDynamic(this, &ARespawnTree::Interact);
}

void ARespawnTree::Interact(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent)
{
	if (bIsActivated)
	{
		return;
	}

	AFirstPersonCharacter* Player = Cast<AFirstPersonCharacter>(Pawn);
	if (!Player)
	{
		return;
	}

	bIsActivated = true;
	SetActive();
	SetRespawnPoint(Player, true);
}

void ARespawnTree::SetActive()
{
	Interaction->RemoveInteractable(TreeModel);
	Interaction->OnInteract.RemoveDynamic(this, &ARespawnTree::Interact);

	FCTween::Play(
		0.f,
		100.f,
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
	UAkGameplayStatics::PostEventAtLocation(ActivationNoise, TreeModel->GetComponentLocation(), TreeModel->GetComponentRotation(), this);

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
