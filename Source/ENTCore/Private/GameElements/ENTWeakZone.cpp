// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTWeakZone.h"
#include "AkGameplayStatics.h"
#include "FCTween.h"
#include "Components/BoxComponent.h"
#include "ENTInteractableComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameElements/ENTAmberOre.h"
#include "Kismet/KismetMathLibrary.h"
#include "Config/ENTCoreConfig.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/States/ENTCharacterStateMachine.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTWorldSave.h"

#if WITH_EDITORONLY_DATA
#include "Components/BillboardComponent.h"
#endif

AENTWeakZone::AENTWeakZone()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("WeakZone"));
	BoxComponent->SetupAttachment(RootComponent);

	BlackAndWhiteShader = CreateDefaultSubobject<UPostProcessComponent>(TEXT("Black and White Shader"));
	BlackAndWhiteShader->SetupAttachment(RootComponent);

#if WITH_EDITORONLY_DATA
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	BillboardComponent->SetupAttachment(RootComponent);
#endif

	Interactable = CreateDefaultSubobject<UENTInteractableComponent>(TEXT("Interactable"));
	Interactable->SetInteractionName(NSLOCTEXT("Actions", "DropAmber", "Drop Amber"));
}

void AENTWeakZone::BeginPlay()
{
	Super::BeginPlay();

	const UENTCoreConfig* CoreConfig =  GetDefault<UENTCoreConfig>();
	if (!CoreConfig)
	{
		return;
	}

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AENTWeakZone::OnZoneBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AENTWeakZone::OnZoneEndOverlap);

	for (const FENTInteractionPoints& InteractionPoint : InteractionPoints)
	{
		InteractionPoint.AmberMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InteractionPoint.InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		InteractionPoint.InteractionBox->SetCollisionResponseToChannel(CoreConfig->InteractionTraceChannel, ECR_Block);

		if (!InteractionPoint.bIsActive)
		{
			Interactable->AddInteractable(InteractionPoint.InteractionBox);
		}
	}

	Interactable->OnInteract.AddDynamic(this, &AENTWeakZone::OnInteract);

	// short delay because GetOverlappingActors does not work properly at the BeginPlay
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AENTWeakZone::InitZone, 0.2f, false);
}

void AENTWeakZone::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BoxComponent->SetBoxExtent(ZoneSize);

	for (FENTInteractionPoints& InteractionPoint : InteractionPoints)
	{
		if (!InteractionPoint.Mesh)
		{
			InteractionPoint.Mesh = DefaultInteractionPointMesh;
		}

		UActorComponent* DefaultComp = AddComponentByClass(UStaticMeshComponent::StaticClass(), true, InteractionPoint.Transform, false);
		UActorComponent* AmberComp = AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform::Identity, false);
		UActorComponent* BoxComp = AddComponentByClass(UBoxComponent::StaticClass(), true, FTransform::Identity, false);
		if (!DefaultComp || !AmberComp || !BoxComp)
		{
			continue;
		}

		UStaticMeshComponent* DefaultMeshComp = Cast<UStaticMeshComponent>(DefaultComp);
		UStaticMeshComponent* AmberMeshComp = Cast<UStaticMeshComponent>(AmberComp);
		UBoxComponent* InteractionComp = Cast<UBoxComponent>(BoxComp);
		if (!DefaultMeshComp || !AmberMeshComp || !InteractionComp)
		{
			continue;
		}

		InteractionPoint.MeshComp = DefaultMeshComp;
		InteractionPoint.AmberMeshComp = AmberMeshComp;
		InteractionPoint.InteractionBox = InteractionComp;

		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
		InteractionPoint.MeshComp->AttachToComponent(RootComponent, AttachmentRules);
		InteractionPoint.AmberMeshComp->AttachToComponent(InteractionPoint.MeshComp, AttachmentRules);
		InteractionPoint.InteractionBox->AttachToComponent(InteractionPoint.MeshComp, AttachmentRules);

		InteractionPoint.MeshComp->SetStaticMesh(InteractionPoint.Mesh);
		InteractionPoint.AmberMeshComp->SetStaticMesh(AmberMesh);
		InteractionPoint.InteractionBox->SetBoxExtent(InteractionBoxExtent, false);
		InteractionPoint.InteractionBox->SetRelativeLocation(FVector(0.0f, 0.0f, InteractionBoxExtent.Z));
	}
}

void AENTWeakZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	for (FENTInteractionPoints& InteractionPoint : InteractionPoints)
	{
		if (!InteractionPoint.bIsActive)
		{
			continue;
		}

		FVector CurrentLocation = InteractionPoint.AmberMeshComp->GetRelativeLocation();
		FVector TargetLocation = FVector(0.0f, 0.0f, TargetAmberHeight);
		float Alpha = DeltaSeconds * AmberAnimSpeed;

		FVector ResultLocation = UKismetMathLibrary::VLerp(CurrentLocation, TargetLocation, Alpha);
		InteractionPoint.AmberMeshComp->SetRelativeLocation(ResultLocation);
	}
}

void AENTWeakZone::InitZone()
{
	TArray<AActor*> OverlappingActors;
	BoxComponent->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor == nullptr)
		{
			continue;
		}

		if(OverlappingActor->Implements<UENTWeakZoneInterface>())
		{
			IENTWeakZoneInterface::Execute_OnEnterWeakZone(OverlappingActor, bIsZoneActive);
		}
	}
}

void AENTWeakZone::DestroyZone()
{
	TArray<AActor*> OverlappingActors;
	BoxComponent->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor == nullptr)
		{
			continue;
		}

		if(OverlappingActor->Implements<UENTWeakZoneInterface>())
		{
			IENTWeakZoneInterface::Execute_OnExitWeakZone(OverlappingActor);
		}
	}

	bIsZoneActive = false;
	BoxComponent->DestroyComponent();
}

void AENTWeakZone::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->Implements<UENTWeakZoneInterface>())
	{
		IENTWeakZoneInterface::Execute_OnEnterWeakZone(OtherActor, bIsZoneActive);
	}
}

void AENTWeakZone::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->Implements<UENTWeakZoneInterface>())
	{
		IENTWeakZoneInterface::Execute_OnExitWeakZone(OtherActor);
	}
}

void AENTWeakZone::OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent)
{
	if (!Pawn)
	{
		return;
	}

	AENTDefaultCharacter* Character = Cast<AENTDefaultCharacter>(Pawn);
	if (!Character)
	{
		return;
	}

	if (!Character->HasRequiredQuantity(EAmberType::WeakAmber, CostByPoint))
	{
		return;
	}

	if (Character->GetStateMachine())
	{
		// play the spike animation
		Character->GetStateMachine()->ChangeState(EENTCharacterStateID::Anim);
	}

	UAkGameplayStatics::PostEvent(GrowlNoise, nullptr, 0, FOnAkPostEventCallback());
	Character->UseAmber(AmberType, CostByPoint);

	FENTInteractionPoints* InteractionPoint = FindInteractionPoint(InteractionComponent);
	if (!InteractionPoint)
	{
		return;
	}

	UAkGameplayStatics::PostEventAtLocation(InjectAmberNoise, InteractionPoint->Transform.GetLocation(), InteractionPoint->Transform.GetRotation().Rotator(), this);
	InteractionPoint->bIsActive = true;
	Interactable->RemoveInteractable(InteractionPoint->InteractionBox);

	CheckIfEveryInteractionsPointActive();
}

FENTInteractionPoints* AENTWeakZone::FindInteractionPoint(TObjectPtr<UPrimitiveComponent> Comp)
{
	for (FENTInteractionPoints& InteractionPoint : InteractionPoints)
	{
		if (InteractionPoint.InteractionBox == Comp)
		{
			return &InteractionPoint;
		}
	}

	return nullptr;
}

bool AENTWeakZone::IsEveryInteractionPointsActive() const
{
	for (const FENTInteractionPoints& InteractionPoint : InteractionPoints)
	{
		if (!InteractionPoint.bIsActive)
		{
			return false;
		}
	}

	return true;
}

void AENTWeakZone::CheckIfEveryInteractionsPointActive()
{
	if (!IsEveryInteractionPointsActive())
	{
		return;
	}

	OnCure.Broadcast();
	FCTween::Play(
		1.f,
		0.f,
		[&](float X)
		{
			MaterialBlackAndWhite->SetScalarParameterValue("Active", X);
		},
		CureDuration,
		EFCEase::OutCubic
	);
}

FENTGameElementData& AENTWeakZone::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	FENTWeakZoneData Data = FENTWeakZoneData();

	Data.ActivatedInteractionPoints.Empty(InteractionPoints.Num());
	for (const FENTInteractionPoints& InteractionPoint : InteractionPoints)
	{
		Data.ActivatedInteractionPoints.Add(InteractionPoint.bIsActive);
	}

	return CurrentWorldSave->WeakZoneData.Add(GetName(), Data);
}

void AENTWeakZone::LoadGameElement(const FENTGameElementData& GameElementData)
{
	const FENTWeakZoneData& Data = static_cast<const FENTWeakZoneData&>(GameElementData);

	if (InteractionPoints.Num() != Data.ActivatedInteractionPoints.Num())
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("%s has a different number in the world and in the save, please clear your save file"), *GetActorLabel(false));

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	for (int i = 0; i < Data.ActivatedInteractionPoints.Num(); i++)
	{
		InteractionPoints[i].bIsActive = Data.ActivatedInteractionPoints[i];
	}
}
