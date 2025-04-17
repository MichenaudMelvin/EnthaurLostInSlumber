// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/Muscle.h"
#include "Components/InteractableComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/FirstPersonCharacter.h"

#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#endif

AMuscle::AMuscle()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Static);

	MuscleMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MuscleMeshComp->SetupAttachment(Root);
	MuscleMeshComp->SetMobility(EComponentMobility::Stationary);
	MuscleMeshComp->SetCollisionObjectType(ECC_WorldStatic);
	MuscleMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MuscleMeshComp->SetCollisionResponseToAllChannels(ECR_Block);

	ObjectsToCheck.Add(ObjectTypeQuery1);
	ObjectsToCheck.Add(ObjectTypeQuery2);
	ObjectsToCheck.Add(ObjectTypeQuery3);
	ObjectsToCheck.Add(ObjectTypeQuery4);

#if WITH_EDITORONLY_DATA
	TraceExtentVisibility = CreateDefaultSubobject<UBoxComponent>(TEXT("TraceExtent"));
	TraceExtentVisibility->SetupAttachment(MuscleMeshComp);
	TraceExtentVisibility->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TraceExtentVisibility->SetCollisionResponseToAllChannels(ECR_Ignore);
	TraceExtentVisibility->SetMobility(EComponentMobility::Static);
	TraceExtentVisibility->bIsEditorOnly = true;

	BounceDirectionTop = CreateDefaultSubobject<UArrowComponent>(TEXT("BounceDirectionTop"));
	BounceDirectionTop->SetupAttachment(MuscleMeshComp);
	BounceDirectionTop->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	BounceDirectionTop->bIsEditorOnly = true;

	BounceDirectionBack = CreateDefaultSubobject<UArrowComponent>(TEXT("BounceDirectionBack"));
	BounceDirectionBack->SetupAttachment(MuscleMeshComp);
	BounceDirectionBack->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	BounceDirectionBack->bIsEditorOnly = true;
#endif

	SpikeInteraction = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikeInteraction"));
	SpikeInteraction->SetupAttachment(Root);
	SpikeInteraction->SetCollisionObjectType(ECC_WorldStatic);
	SpikeInteraction->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SpikeInteraction->SetCollisionResponseToAllChannels(ECR_Block);
	SpikeInteraction->SetMobility(EComponentMobility::Static);

	Interactable = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));

	SolidMuscleInteraction = NSLOCTEXT("Interactions", "SolidMuscleInteraction", "Make soft");
	SoftMuscleInteraction = NSLOCTEXT("Interactions", "SoftMuscleInteraction", "Make solid");
}

void AMuscle::BeginPlay()
{
	Super::BeginPlay();

	bDefaultSolidity = bIsSolid;
	CurrentZScale = MuscleHeight.GetUpperBoundValue();

	DeformationSpeed = 1 / DeformationSpeed;

	bTriggerDeformation = false;
	Interactable->AddInteractable(SpikeInteraction);
	Interactable->OnInteract.AddDynamic(this, &AMuscle::Interact);

	if (MuscleStateTransitionCurve)
	{
		FOnTimelineFloat UpdateEvent;
		UpdateEvent.BindDynamic(this, &AMuscle::UpdateMuscleStateTransition);
		MuscleStateTransitionTimeline.AddInterpFloat(MuscleStateTransitionCurve, UpdateEvent);
		MuscleStateTransitionTimeline.SetPlayRate(1/MuscleStateTransitionDuration);
	}

	FBox BoundingBox = MuscleMeshComp->GetStaticMesh()->GetBoundingBox();
	FVector MeshSize = BoundingBox.Max - BoundingBox.Min;
	MuscleZUnit = MeshSize.Z * 0.5f;

	if (!DynamicMaterial)
	{
		DynamicMaterial = MuscleMeshComp->CreateDynamicMaterialInstance(0, MuscleMaterial);
		MuscleMeshComp->SetMaterial(0, DynamicMaterial);
	}

	MuscleMeshComp->OnComponentHit.AddDynamic(this, &AMuscle::HitMuscleMesh);

	UpdateMuscleSolidity();
}

void AMuscle::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	bDefaultSolidity = bIsSolid;
	CurrentZScale = MuscleHeight.GetUpperBoundValue();

	if (!MuscleMeshComp)
	{
		return;
	}

	MuscleMeshComp->SetStaticMesh(MuscleMesh);

	RebuildMuscleMesh();

	if (!DynamicMaterial)
	{
		DynamicMaterial = MuscleMeshComp->CreateDynamicMaterialInstance(0, MuscleMaterial);
		MuscleMeshComp->SetMaterial(0, DynamicMaterial);
	}

	UpdateMuscleStateTransition(bIsSolid ? 0.0f : 1.0f);
}

void AMuscle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MuscleStateTransitionTimeline.TickTimeline(DeltaTime);
	DeformMesh(DeltaTime);
}

void AMuscle::ToggleMuscleSolidity()
{
	if (bIsLocked)
	{
		return;
	}

	bIsSolid = !bIsSolid;
	UpdateMuscleSolidity();
}

void AMuscle::HitMuscleMesh(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HitMuscle(OtherActor, OtherComp);
}

void AMuscle::StartDeformation()
{
	DeformationDirection = 1;
	bTriggerDeformation = true;
}

void AMuscle::EndDeformation()
{
	DeformationDirection = -1;
	bTriggerDeformation = true;
}

void AMuscle::DeformMesh(float DeltaTime)
{
	if (!DeformationCurve)
	{
		return;
	}

	if (bTriggerDeformation && !bIsSolid)
	{
		DeformationAlpha = (DeltaTime * DeformationDirection * DeformationSpeed) + DeformationAlpha;
		DeformationAlpha = FMath::Clamp(DeformationAlpha, 0, DeformationDuration);

		float Time = UKismetMathLibrary::NormalizeToRange(DeformationAlpha, 0.0f, DeformationDuration);
		float Alpha = DeformationCurve->GetFloatValue(Time);
		CurrentZScale = FMath::Lerp(MuscleHeight.GetUpperBoundValue(), MuscleHeight.GetLowerBoundValue(), Alpha);

		CurrentZScale = FMath::Clamp(CurrentZScale, MinPossibleScale, INFINITY);

		RebuildMuscleMesh();

		if (DeformationAlpha == 0.0f || DeformationAlpha == DeformationDuration)
		{
			bTriggerDeformation = false;

			EndDeformation();
		}
	}
	else
	{
		if (!IsDeformed())
		{
			EndDeformation();
		}
	}
}

bool AMuscle::IsDeformed() const
{
	return CurrentZScale != MuscleHeight.GetUpperBoundValue();
}

void AMuscle::RebuildMuscleMesh() const
{
	if (!MuscleMeshComp)
	{
		return;
	}

	MuscleMeshComp->SetRelativeScale3D(FVector(MuscleSize.X, MuscleSize.Y, CurrentZScale));

#if WITH_EDITORONLY_DATA
	FBox BoundingBox = MuscleMeshComp->GetStaticMesh()->GetBoundingBox();
	FVector MeshSize = (BoundingBox.Max - BoundingBox.Min) * MuscleMeshComp->GetRelativeScale3D();
	MeshSize *= 0.5;

	BounceDirectionTop->SetRelativeLocation(FVector(0.0f, 0.0f, (MeshSize.Z / CurrentZScale)));
	BounceDirectionBack->SetRelativeLocation(FVector(0.0f, 0.0f, -(MeshSize.Z / CurrentZScale)));

	TraceExtentVisibility->SetBoxExtent(MeshSize + (TraceExtent * MuscleMeshComp->GetRelativeScale3D() * 0.5f));
#endif
}

void AMuscle::UpdateMuscleSolidity()
{
	if (bIsSolid)
	{
		MuscleMeshComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Default, 0.0f));
		Interactable->SetInteractionName(SolidMuscleInteraction);
		MuscleStateTransitionTimeline.Reverse();
	}
	else
	{
		MuscleMeshComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.0f));
		Interactable->SetInteractionName(SoftMuscleInteraction);
		MuscleStateTransitionTimeline.Play();
	}
}

void AMuscle::HitMuscle(AActor* HitActor, UPrimitiveComponent* OtherComp)
{
	if (!HitActor)
	{
		return;
	}

	if (bIsSolid)
	{
		UpdateMuscleSolidity();
		return;
	}

	if (!IsActorInRange(HitActor))
	{
		return;
	}

	UpdateMuscleSolidity();

	float ActorVelocityLength = OtherComp ? OtherComp->GetComponentVelocity().Size() : HitActor->GetVelocity().Size();

	if (ActorVelocityLength < MinTriggerVelocity)
	{
		return;
	}

	ActorVelocityLength = FMath::Clamp(ActorVelocityLength, MinTriggerVelocity, MaxLaunchVelocity);

	StartDeformation();

	FVector LaunchVelocity = MuscleMeshComp->GetUpVector() * (bUseFixedVelocity ? FixedVelocity : (ActorVelocityLength * VelocityMultiplier));

	AFirstPersonCharacter* Player = Cast<AFirstPersonCharacter>(HitActor);
	if (Player)
	{
		Player->EjectCharacter(LaunchVelocity);
	}
	else
	{
		UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(HitActor->GetRootComponent());
		if (Primitive)
		{
			Primitive->AddForce(LaunchVelocity);
		}
	}
}

bool AMuscle::IsActorInRange(AActor* Actor)
{
	if (!Actor)
	{
		return false;
	}

	FVector MuscleLocation = MuscleMeshComp->GetComponentLocation();
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	TArray<FHitResult> HitResults;

	FBox BoundingBox = MuscleMeshComp->GetStaticMesh()->GetBoundingBox();
	FVector MeshSize = (BoundingBox.Max - BoundingBox.Min) * MuscleMeshComp->GetRelativeScale3D();
	MeshSize *= 0.5;

	FVector TraceSize = MeshSize + (TraceExtent * MuscleMeshComp->GetRelativeScale3D() * 0.5f);

	bool bHit = UKismetSystemLibrary::BoxTraceMultiForObjects(this, MuscleLocation, MuscleLocation, TraceSize, FRotator::ZeroRotator, ObjectsToCheck, false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true);

	if (!bHit)
	{
		return false;
	}

	for (const FHitResult& HitResult : HitResults)
	{
		if (HitResult.GetActor() == Actor)
		{
			return true;
		}
	}

	return false;
}

void AMuscle::UpdateMuscleStateTransition(float Alpha)
{
	if (!DynamicMaterial)
	{
		return;
	}

	DynamicMaterial->SetScalarParameterValue(MuscleStateTransitionParam, Alpha);
}

void AMuscle::Interact(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractComponent)
{
	ToggleMuscleSolidity();
}

void AMuscle::OnActorAbove_Implementation(AActor* Actor)
{
	IGroundAction::OnActorAbove_Implementation(Actor);

	HitMuscle(Actor, nullptr);
}

void AMuscle::OnEnterWeakZone_Implementation(bool bIsZoneActive)
{
	IWeakZoneInterface::OnEnterWeakZone_Implementation(bIsZoneActive);

	if (bIsZoneActive && Interactable->OnInteract.IsAlreadyBound(this, &AMuscle::Interact))
	{
		Interactable->OnInteract.RemoveDynamic(this, &AMuscle::Interact);
	}
}

void AMuscle::OnExitWeakZone_Implementation()
{
	IWeakZoneInterface::OnExitWeakZone_Implementation();

	if (!Interactable->OnInteract.IsAlreadyBound(this, &AMuscle::Interact))
	{
		Interactable->OnInteract.RemoveDynamic(this, &AMuscle::Interact);
	}
}

void AMuscle::Trigger_Implementation()
{
	INerveReactive::Trigger_Implementation();

	ToggleMuscleSolidity();
}

void AMuscle::SetLock_Implementation(bool state)
{
	INerveReactive::SetLock_Implementation(state);

	if (bIsSolid == bDefaultSolidity)
	{
		return;
	}

	Trigger();
	bIsLocked = state;
}
