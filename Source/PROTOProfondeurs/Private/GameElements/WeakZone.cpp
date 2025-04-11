// Fill out your copyright notice in the Description page of Project Settings.


#include "PROTOProfondeurs/Public/GameElements/WeakZone.h"

#include "MovieSceneTracksComponentTypes.h"
#include "Components/BoxComponent.h"
#include "Components/InteractableComponent.h"
#include "GameElements/AmberOre.h"
#include "Physics/TracePhysicsSettings.h"
#include "Player/FirstPersonCharacter.h"

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

	for (UStaticMeshComponent* Mesh : InteractionPoints)
	{
		Mesh->SetCollisionResponseToChannel(TraceSettings->InteractionTraceChannel, ECR_Block);
		Interactable->AddInteractable(Mesh);
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

	InteractionPoints.Empty();

	if (InteractionMesh == nullptr)
	{
		return;
	}

	for (const FTransform& TransformPoint : InteractionTransformPoints)
	{
		UActorComponent* Comp = AddComponentByClass(UStaticMeshComponent::StaticClass(), false, TransformPoint, false);

		if (Comp == nullptr)
		{
			continue;
		}

		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Comp); 
		if (StaticMeshComponent == nullptr)
		{
			continue;
		}

		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
		StaticMeshComponent->AttachToComponent(RootComponent, AttachmentRules);
		StaticMeshComponent->SetStaticMesh(InteractionMesh);
		InteractionPoints.Add(StaticMeshComponent);
	}
}

void AWeakZone::Destroyed()
{
	Super::Destroyed();

	DestroyZone();
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

	TArray<UPrimitiveComponent*> Components;
	BoxComponent->GetOverlappingComponents(Components);

	AllMaterialInstances.Empty();

	for (UPrimitiveComponent* Component : Components)
	{
		for (int i = 0; i < Component->GetNumMaterials(); i++)
		{
			UMaterialInterface* Material = Component->GetMaterial(i);
			UMaterialInstanceDynamic* DynamicMaterial = Component->CreateDynamicMaterialInstance(i, Material);

			if (DynamicMaterial == nullptr)
			{
				continue;
			}

			DynamicMaterial->SetVectorParameterValue(ZoneLocationParamName, BoxComponent->GetComponentLocation());
			DynamicMaterial->SetVectorParameterValue(ZoneExtentParamName, BoxComponent->GetScaledBoxExtent() * 2.0f);

			AllMaterialInstances.Add(DynamicMaterial);
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

	for (UMaterialInstanceDynamic* Instance : AllMaterialInstances)
	{
		if (Instance == nullptr)
		{
			continue;
		}

		Instance->SetVectorParameterValue(ZoneLocationParamName, FVector::ZeroVector);
		Instance->SetVectorParameterValue(ZoneExtentParamName, FVector::ZeroVector);
	}

	AllMaterialInstances.Empty();
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

void AWeakZone::OnInteract(APlayerController* Controller, APawn* Pawn)
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

	if (!Character->IsAmberTypeFilled(EAmberType::WeakAmber))
	{
		return;
	}

	Character->UseAmber(EAmberType::WeakAmber, 1);
	Destroy();
}

