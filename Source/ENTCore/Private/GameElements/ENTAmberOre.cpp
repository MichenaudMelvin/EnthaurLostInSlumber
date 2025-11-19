// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTAmberOre.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "Components/BoxComponent.h"
#include "ENTInteractableComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameElements/ENTNerveReceptacle.h"
#include "GameElements/ENTWeakZone.h"
#include "Interface/ENTActivation.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
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

	Foliage = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Foliage"));
	Foliage->SetupAttachment(Mesh);
	Foliage->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Foliage->SetMobility(EComponentMobility::Stationary);
}

void AENTAmberOre::BeginPlay()
{
	Super::BeginPlay();
	
	Interactable->AddInteractable(MeshInteraction);
	Interactable->OnInteract.AddDynamic(this, &AENTAmberOre::OnInteract);

#if WITH_EDITORONLY_DATA

	if (!Foliage)
	{
		return;
	}

	for (int32 i = 0; i < Foliage->GetInstanceCount(); i++)
	{
		FTransform InstanceTransform;
		Foliage->GetInstanceTransform(i, InstanceTransform, false);
		InstanceTransform.SetScale3D(FVector::ZeroVector);

		Foliage->UpdateInstanceTransform(i, InstanceTransform, false, false, false);
	}

	Foliage->MarkRenderStateDirty();

#endif

	FOnTimelineFloat UpdateEvent;

	UpdateEvent.BindDynamic(this, &AENTAmberOre::FoliageGrowthUpdate);
	FoliageTimeline.AddInterpFloat(FoliageGrowthCurve, UpdateEvent);
	FoliageTimeline.SetPlayRate(1 / GrowthDuration);

	UpdateEvent.Unbind();
}

void AENTAmberOre::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Mesh->SetStaticMesh(SourceMesh);

	TargetAmberHeight = bIsEmpty? EmptyAmberHeight : FullAmberHeight;
	FVector ResultLocation = AmberMesh->GetRelativeLocation();
	ResultLocation.Z = TargetAmberHeight;
	AmberMesh->SetRelativeLocation(ResultLocation);

	Foliage->SetStaticMesh(FoliageMesh);
	Foliage->ClearInstances();

#if WITH_EDITORONLY_DATA
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
#endif
	
	if (!Foliage)
	{
		return;
	}

	FVector Origin = GetTransform().GetLocation();
	float OriginHeight = Origin.Z;
	Origin.Z += TraceLength;

	for (uint16 i = 0; i < MeshesNumberByInteractionsPoints; i++)
	{
		FVector Direction = UKismetMathLibrary::RandomUnitVectorFromStream(Seed);
		float DirectionLength = UKismetMathLibrary::RandomFloatInRangeFromStream(Seed, FoliageOffsetRange.GetLowerBoundValue(), FoliageOffsetRange.GetUpperBoundValue());

		FVector EndTrace = (Direction * DirectionLength) + Origin;
		EndTrace.Z = OriginHeight - TraceLength;

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		FHitResult Hit;
		bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, Origin, EndTrace, ObjectsTypes, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);

#if WITH_EDITORONLY_DATA
		if (bShowTraces)
		{
			UKismetSystemLibrary::DrawDebugPoint(this, Origin, TracesSize, FLinearColor::Green, INFINITY);
			UKismetSystemLibrary::DrawDebugPoint(this, EndTrace, TracesSize, FLinearColor::Green, INFINITY);
			UKismetSystemLibrary::DrawDebugLine(this, Origin, EndTrace, FLinearColor::Green, INFINITY);

			if (bHit)
			{
				UKismetSystemLibrary::DrawDebugPoint(this, Hit.Location, TracesSize, FLinearColor::Red, INFINITY);
			}
		}
#endif

		if (!bHit)
		{
			continue;
		}

		float RandomAngle = UKismetMathLibrary::RandomFloatInRangeFromStream(Seed, 0.0f, 360.0f);

		FVector TargetFoliageScale = FVector::ZeroVector;

#if WITH_EDITORONLY_DATA
		if (bShowFoliage)
		{
			TargetFoliageScale = FoliageScale;
		}
#endif

		FTransform FoliageTransform(FRotator(0.0f, RandomAngle, 0.0f), Hit.Location, TargetFoliageScale);
		Foliage->AddInstance(FoliageTransform, true);
	}
}

void AENTAmberOre::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector CurrentLocation = AmberMesh->GetRelativeLocation();
	FVector TargetLocation = FVector(0.0f, 0.0f, TargetAmberHeight);
	float Alpha = DeltaSeconds * AmberAnimSpeed;

	FVector ResultLocation = UKismetMathLibrary::VLerp(CurrentLocation, TargetLocation, Alpha);
	AmberMesh->SetRelativeLocation(ResultLocation);

	FoliageTimeline.TickTimeline(DeltaSeconds);
}

void AENTAmberOre::OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent)
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
	
	if (bIsEmpty)
	{
		if (!Character->HasRequiredQuantity(EAmberType::WeakAmber, 1))
		{
			return;
		}

		if (Character->GetStateMachine())
		{
			// play the spike animation
			Character->GetStateMachine()->ChangeState(EENTCharacterStateID::Anim);
		}

		UAkGameplayStatics::PostEvent(GrowlNoise, nullptr, 0, FOnAkPostEventCallback());
		Character->UseAmber(AmberType, 1);

		UAkGameplayStatics::PostEventAtLocation(InjectAmberNoise, GetTransform().GetLocation(), GetTransform().GetRotation().Rotator(), this);

		TargetAmberHeight = FullAmberHeight;
		bIsEmpty = !bIsEmpty;

		UAkGameplayStatics::PostEventAtLocation(FoliageGrowthNoise, GetTransform().GetLocation(),GetTransform().Rotator(), this);
		if (!LinkedWeakZone) return;
		LinkedWeakZone->CureZone(this);
		FoliageTimeline.Play();
		TriggerLinkedObjects();
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
	Character->MineAmber(AmberType, 1);
	TargetAmberHeight = EmptyAmberHeight;

	bIsEmpty = !bIsEmpty;

}

void AENTAmberOre::FoliageGrowthUpdate(float Alpha)
{
	if (!Foliage)
	{
		return;
	}
	
	FVector TargetScale = UKismetMathLibrary::VLerp(FVector::ZeroVector, FoliageScale, Alpha);

	for (int32 i = 0; i < Foliage->GetInstanceCount(); i++)
	{
		FTransform InstanceTransform;
		Foliage->GetInstanceTransform(i, InstanceTransform, true);
		InstanceTransform.SetScale3D(TargetScale);

		Foliage->UpdateInstanceTransform(i, InstanceTransform, true, false, false);
	}
	
	Foliage->MarkRenderStateDirty();
}

void AENTAmberOre::TriggerLinkedObjects()
{
	TArray<AActor*> Actors;
	ObjectReactive.GetKeys(Actors);

	for (auto Actor : Actors)
	{
		if (Actor->Implements<UENTActivation>())
		{
			if (!IsValid(Actor))
			{
				continue;
			}

			if (Actor->Implements<UENTActivation>())
			{
				if (ObjectReactive[Actor] == ENerveReactiveInteractionType::ForceDefaultState)
				{
					IENTActivation::Execute_SetLock(Actor, true);
				} else
				{
					IENTActivation::Execute_Trigger(Actor);
				}
			}
		}
	}
}

FENTGameElementData& AENTAmberOre::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	FENTAmberOreData Data;
	Data.bIsEmpty = bIsEmpty;

	return CurrentWorldSave->AmberOreData.Add(GetName(), Data);
}

void AENTAmberOre::LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave)
{
	const FENTAmberOreData& Data = static_cast<const FENTAmberOreData&>(GameElementData);
	bIsEmpty = Data.bIsEmpty;
	TargetAmberHeight = bIsEmpty? EmptyAmberHeight : FullAmberHeight;
	FVector ResultLocation = AmberMesh->GetRelativeLocation();
	ResultLocation.Z = TargetAmberHeight;
	AmberMesh->SetRelativeLocation(ResultLocation);
}
