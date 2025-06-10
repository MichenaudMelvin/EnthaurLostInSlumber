// Fill out your copyright notice in the Description page of Project Settings.


#include "PROTOProfondeurs/Public/GameElements/WeakZone.h"

#include "AkGameplayStatics.h"
#include "FCTween.h"
#include "Components/BoxComponent.h"
#include "Components/InteractableComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameElements/AmberOre.h"
#include "Kismet/KismetMathLibrary.h"
#include "Physics/TracePhysicsSettings.h"
#include "Player/FirstPersonCharacter.h"
#include "Saves/WorldSaves/WorldSave.h"

#if WITH_EDITORONLY_DATA
#include "Components/BillboardComponent.h"
#endif

AWeakZone::AWeakZone()
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

	Interactable = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	Interactable->SetInteractionName(NSLOCTEXT("Actions", "DropAmber", "Drop Amber"));
}

void AWeakZone::BeginPlay()
{
	Super::BeginPlay();

	const UTracePhysicsSettings* TraceSettings =  GetDefault<UTracePhysicsSettings>();

	if (TraceSettings == nullptr)
	{
		return;
	}

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeakZone::OnZoneBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AWeakZone::OnZoneEndOverlap);

	for (const FInteractionPoints& InteractionPoint : InteractionPoints)
	{
		InteractionPoint.MeshComp->SetCollisionResponseToChannel(TraceSettings->InteractionTraceChannel, ECR_Block);
		InteractionPoint.AmberMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (!InteractionPoint.bIsActive)
		{
			Interactable->AddInteractable(InteractionPoint.MeshComp);
		}
	}

	Interactable->OnInteract.AddDynamic(this, &AWeakZone::OnInteract);

	// short delay because GetOverlappingActors does not work properly at the BeginPlay
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AWeakZone::InitZone, 0.2f, false);
}

void AWeakZone::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BoxComponent->SetBoxExtent(ZoneSize);

	for (FInteractionPoints& InteractionPoint : InteractionPoints)
	{
		if (!InteractionPoint.Mesh)
		{
			InteractionPoint.Mesh = DefaultInteractionPointMesh;
		}

		UActorComponent* DefaultComp = AddComponentByClass(UStaticMeshComponent::StaticClass(), true, InteractionPoint.Transform, false);
		UActorComponent* AmberComp = AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform::Identity, false);
		if (!DefaultComp || !AmberComp)
		{
			continue;
		}

		UStaticMeshComponent* DefaultMeshComp = Cast<UStaticMeshComponent>(DefaultComp);
		UStaticMeshComponent* AmberMeshComp = Cast<UStaticMeshComponent>(AmberComp);
		if (!DefaultMeshComp || !AmberMeshComp)
		{
			continue;
		}

		InteractionPoint.MeshComp = DefaultMeshComp;
		InteractionPoint.AmberMeshComp = AmberMeshComp;

		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
		InteractionPoint.MeshComp->AttachToComponent(RootComponent, AttachmentRules);
		InteractionPoint.AmberMeshComp->AttachToComponent(InteractionPoint.MeshComp, AttachmentRules);

		InteractionPoint.MeshComp->SetStaticMesh(InteractionPoint.Mesh);
		InteractionPoint.AmberMeshComp->SetStaticMesh(AmberMesh);
	}
}

void AWeakZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	for (FInteractionPoints& InteractionPoint : InteractionPoints)
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

void AWeakZone::InitZone()
{
	TArray<AActor*> OverlappingActors;
	BoxComponent->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor == nullptr)
		{
			continue;
		}

		if(OverlappingActor->Implements<UWeakZoneInterface>())
		{
			IWeakZoneInterface::Execute_OnEnterWeakZone(OverlappingActor, bIsZoneActive);
		}
	}
}

void AWeakZone::DestroyZone()
{
	TArray<AActor*> OverlappingActors;
	BoxComponent->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor == nullptr)
		{
			continue;
		}

		if(OverlappingActor->Implements<UWeakZoneInterface>())
		{
			IWeakZoneInterface::Execute_OnExitWeakZone(OverlappingActor);
		}
	}

	bIsZoneActive = false;
	BoxComponent->DestroyComponent();
}

void AWeakZone::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->Implements<UWeakZoneInterface>())
	{
		IWeakZoneInterface::Execute_OnEnterWeakZone(OtherActor, bIsZoneActive);
	}
}

void AWeakZone::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->Implements<UWeakZoneInterface>())
	{
		IWeakZoneInterface::Execute_OnExitWeakZone(OtherActor);
	}
}

void AWeakZone::OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent)
{
	if (!Pawn)
	{
		return;
	}

	AFirstPersonCharacter* Character = Cast<AFirstPersonCharacter>(Pawn);
	if (!Character)
	{
		return;
	}

	if (!Character->HasRequiredQuantity(EAmberType::WeakAmber, CostByPoint))
	{
		return;
	}

	UAkGameplayStatics::PostEvent(GrowlNoise, nullptr, 0, FOnAkPostEventCallback());
	Character->UseAmber(AmberType, CostByPoint);

	FInteractionPoints* InteractionPoint = FindInteractionPoint(InteractionComponent);
	if (!InteractionPoint)
	{
		return;
	}

	UAkGameplayStatics::PostEventAtLocation(InjectAmberNoise, InteractionPoint->Transform.GetLocation(), InteractionPoint->Transform.GetRotation().Rotator(), this);
	InteractionPoint->bIsActive = true;
	Interactable->RemoveInteractable(InteractionPoint->MeshComp);

	CheckIfEveryInteractionsPointActive();
}

FInteractionPoints* AWeakZone::FindInteractionPoint(TObjectPtr<UPrimitiveComponent> Comp)
{
	for (FInteractionPoints& InteractionPoint : InteractionPoints)
	{
		if (InteractionPoint.MeshComp == Comp)
		{
			return &InteractionPoint;
		}
	}

	return nullptr;
}

bool AWeakZone::IsEveryInteractionPointsActive() const
{
	for (const FInteractionPoints& InteractionPoint : InteractionPoints)
	{
		if (!InteractionPoint.bIsActive)
		{
			return false;
		}
	}

	return true;
}

void AWeakZone::CheckIfEveryInteractionsPointActive()
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

FGameElementData& AWeakZone::SaveGameElement(UWorldSave* CurrentWorldSave)
{
	FWeakZoneData Data = FWeakZoneData();

	Data.ActivatedInteractionPoints.Empty(InteractionPoints.Num());
	for (const FInteractionPoints& InteractionPoint : InteractionPoints)
	{
		Data.ActivatedInteractionPoints.Add(InteractionPoint.bIsActive);
	}

	return CurrentWorldSave->WeakZoneData.Add(GetName(), Data);
}

void AWeakZone::LoadGameElement(const FGameElementData& GameElementData)
{
	const FWeakZoneData& Data = static_cast<const FWeakZoneData&>(GameElementData);

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
