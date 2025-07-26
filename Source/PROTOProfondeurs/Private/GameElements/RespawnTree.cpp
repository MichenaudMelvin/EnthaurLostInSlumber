// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/RespawnTree.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "Components/BoxComponent.h"
#include "GameModes/FirstPersonGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTWorldSave.h"
#include "Subsystems/ENTWorldSaveSubsystem.h"

ARespawnTree::ARespawnTree()
{
	PrimaryActorTick.bCanEverTick = false;

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
}

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
		Material->SetScalarParameterValue("Emissive", 0.0f);
	}
}

void ARespawnTree::Destroyed()
{
	Super::Destroyed();

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

	TriggerBox->OnComponentBeginOverlap.RemoveDynamic(this, &ARespawnTree::TriggerEnter);
}

void ARespawnTree::SetActive_Implementation()
{
	bIsActivated = true;

	RespawnTreeNoises->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
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

	if (bIsZoneActive && TriggerBox->OnComponentBeginOverlap.IsAlreadyBound(this, &ARespawnTree::TriggerEnter))
	{
		TriggerBox->OnComponentBeginOverlap.RemoveDynamic(this, &ARespawnTree::TriggerEnter);
	}
}

void ARespawnTree::OnExitWeakZone_Implementation()
{
	IWeakZoneInterface::OnExitWeakZone_Implementation();

	if (!TriggerBox->OnComponentBeginOverlap.IsAlreadyBound(this, &ARespawnTree::TriggerEnter))
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ARespawnTree::TriggerEnter);
	}
}

FENTGameElementData& ARespawnTree::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	FENTRespawnTreeData Data;
	Data.bIsActive = bIsActivated;

	return CurrentWorldSave->RespawnTreeData.Add(GetName(), Data);
}

void ARespawnTree::LoadGameElement(const FENTGameElementData& GameElementData)
{
	const FENTRespawnTreeData& Data = static_cast<const FENTRespawnTreeData&>(GameElementData);
	bIsActivated = Data.bIsActive;

	UENTWorldSaveSubsystem* WorldSaveSubsystem = GetGameInstance()->GetSubsystem<UENTWorldSaveSubsystem>();
	if(!WorldSaveSubsystem)
	{
		return;
	}

	LastCheckPointName = WorldSaveSubsystem->GetCurrentWorldSave()->LastCheckPointName;
}
