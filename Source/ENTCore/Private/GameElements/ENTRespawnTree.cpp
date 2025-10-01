// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTRespawnTree.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "Components/BoxComponent.h"
#include "GameModes/ENTDefaultGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ENTDefaultCharacter.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTWorldSave.h"
#include "Subsystems/ENTWorldSaveSubsystem.h"

AENTRespawnTree::AENTRespawnTree()
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

void AENTRespawnTree::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AENTRespawnTree::TriggerEnter);

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

		AENTDefaultCharacter* Player = Cast<AENTDefaultCharacter>(Character);
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

void AENTRespawnTree::Destroyed()
{
	Super::Destroyed();

	if (TriggerBox && TriggerBox->OnComponentBeginOverlap.IsAlreadyBound(this, &AENTRespawnTree::TriggerEnter))
	{
		TriggerBox->OnComponentBeginOverlap.RemoveDynamic(this, &AENTRespawnTree::TriggerEnter);
	}
}

void AENTRespawnTree::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector RelativeLocation = TriggerBox->GetRelativeLocation();
	TriggerBox->SetRelativeLocation(FVector(RelativeLocation.X, RelativeLocation.Y, TriggerBox->GetUnscaledBoxExtent().Z));
}

void AENTRespawnTree::TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AENTDefaultCharacter* Player = Cast<AENTDefaultCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	if (!bIsActivated)
	{
		SetActive();
	}

	SetRespawnPoint(Player, true);

	TriggerBox->OnComponentBeginOverlap.RemoveDynamic(this, &AENTRespawnTree::TriggerEnter);
}

void AENTRespawnTree::SetActive_Implementation()
{
	bIsActivated = true;

	RespawnTreeNoises->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
}

void AENTRespawnTree::SetRespawnPoint(AENTDefaultCharacter* Player, bool bSave)
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

	AENTDefaultGameMode* DefaultGameMode = Cast<AENTDefaultGameMode>(GameMode);
	if (!DefaultGameMode)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Invalid GameMode, please make sure the current GameMode is %s to save the world"), *DefaultGameMode->GetClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	DefaultGameMode->SaveWorld();
}

void AENTRespawnTree::OnEnterWeakZone_Implementation(bool bIsZoneActive)
{
	IENTWeakZoneInterface::OnEnterWeakZone_Implementation(bIsZoneActive);

	if (bIsZoneActive && TriggerBox->OnComponentBeginOverlap.IsAlreadyBound(this, &AENTRespawnTree::TriggerEnter))
	{
		TriggerBox->OnComponentBeginOverlap.RemoveDynamic(this, &AENTRespawnTree::TriggerEnter);
	}
}

void AENTRespawnTree::OnExitWeakZone_Implementation()
{
	IENTWeakZoneInterface::OnExitWeakZone_Implementation();

	if (!TriggerBox->OnComponentBeginOverlap.IsAlreadyBound(this, &AENTRespawnTree::TriggerEnter))
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AENTRespawnTree::TriggerEnter);
	}
}

FENTGameElementData& AENTRespawnTree::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	FENTRespawnTreeData Data;
	Data.bIsActive = bIsActivated;

	return CurrentWorldSave->RespawnTreeData.Add(GetName(), Data);
}

void AENTRespawnTree::LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave)
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
