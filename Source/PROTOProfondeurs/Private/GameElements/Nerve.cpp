// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/Nerve.h"
#include "FCTween.h"
#include "Components/InteractableComponent.h"
#include "GameFramework/Character.h"
#include "GameElements/NerveReceptacle.h"
#include "Components/PlayerToNervePhysicConstraint.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/CharacterSettings.h"
#include "Player/FirstPersonController.h"

ANerve::ANerve()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Static);

	SplineCable = CreateDefaultSubobject<USplineComponent>(TEXT("SplineCable"));
	SplineCable->SetupAttachment(RootComponent);
	SplineCable->SetMobility(EComponentMobility::Static);

	USplineMeshComponent* SplineMesh = CreateDefaultSubobject<USplineMeshComponent>(TEXT("Spline Mesh"));
	SplineMesh->SetMobility(EComponentMobility::Movable);
	SplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SplineMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SplineMesh->SetGenerateOverlapEvents(false);
	SplineMeshes.Add(SplineMesh);

	NerveBall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	NerveBall->SetupAttachment(RootComponent);

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interaction"));
	InteractableComponent->OnInteract.AddDynamic(this, &ANerve::Interaction);

	CableColliders.Add(ObjectTypeQuery1);
	CableColliders.Add(ObjectTypeQuery2);
}

void ANerve::BeginPlay()
{
	Super::BeginPlay();

	InteractableComponent->AddInteractable(NerveBall);
	DefaultNervePosition = NerveBall->GetComponentLocation();

	FOnTimelineFloat UpdateEvent;
	FOnTimelineEvent FinishEvent;
	UpdateEvent.BindDynamic(this, &ANerve::RetractCable);
	FinishEvent.BindDynamic(this, &ANerve::FinishRetractCable);
	RetractTimeline.AddInterpFloat(RetractionCurve, UpdateEvent);
	RetractTimeline.SetTimelineFinishedFunc(FinishEvent);
}

void ANerve::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ResetCables();

	if (StartCableLength > CableMaxExtension)
	{
		CableMaxExtension = StartCableLength + 1000.0f;
	}
}

void ANerve::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ApplyCablesPhysics();
	if (RetractTimeline.IsPlaying())
	{
		RetractTimeline.TickTimeline(DeltaSeconds);
	}
}

#pragma region Cables

void ANerve::AddSplinePoint(const FVector& SpawnLocation, const ESplineCoordinateSpace::Type& CoordinateSpace, bool bCreateSplineMesh)
{
	int Index = SplineCable->GetNumberOfSplinePoints();
	SplineCable->AddSplinePoint(SpawnLocation, CoordinateSpace, false);
	SplineCable->SetTangentAtSplinePoint(Index, FVector::ZeroVector, ESplineCoordinateSpace::Local, false);

	// correct the location of the last spline point
	int LastIndex = Index - 1;
	if (LastIndex > 0)
	{
		SplineCable->SetLocationAtSplinePoint(LastIndex, SpawnLocation, CoordinateSpace, false);
	}

	SplineCable->UpdateSpline();

	if (!bCreateSplineMesh || SplineCable->GetNumberOfSplinePoints() < 2)
	{
		return;
	}

	FVector EndSplineLocation = CoordinateSpace == ESplineCoordinateSpace::World ? NerveBall->GetComponentLocation() : NerveBall->GetRelativeLocation();
	AddSplineMesh(SpawnLocation, EndSplineLocation, CoordinateSpace);
}

void ANerve::AddSplineMesh(const FVector& StartLocation, const FVector& EndLocation, const ESplineCoordinateSpace::Type& CoordinateSpace)
{
	UActorComponent* Comp = AddComponentByClass(USplineMeshComponent::StaticClass(), false, FTransform::Identity, false);
	if (!Comp)
	{
		return;
	}

	USplineMeshComponent* SplineMesh = Cast<USplineMeshComponent>(Comp);
	if (!SplineMesh)
	{
		return;
	}

	SplineMesh->SetMobility(EComponentMobility::Movable);
	SplineMesh->SetStaticMesh(CableMesh);
	SplineMesh->SetMaterial(0, CableMaterial);

	SplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SplineMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SplineMesh->SetGenerateOverlapEvents(false);

	FVector StartSplineLocation;
	FVector EndSplineLocation;
	if (CoordinateSpace == ESplineCoordinateSpace::World)
	{
		StartSplineLocation = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), StartLocation);
		EndSplineLocation = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), EndLocation);
	}
	else
	{
		StartSplineLocation = StartLocation;
		EndSplineLocation = EndLocation;
	}

	SplineMesh->SetForwardAxis(CableForwardAxis, false);

	SplineMesh->SetStartPosition(StartSplineLocation, false);
	SplineMesh->SetEndPosition(EndSplineLocation, false);

	FVector SplineDirection = UKismetMathLibrary::GetDirectionUnitVector(StartSplineLocation, EndSplineLocation);
	SplineMesh->SetStartTangent(SplineDirection, false);
	SplineMesh->SetEndTangent(SplineDirection, false);

	SplineMesh->SetStartScale(CableScale, false);
	SplineMesh->SetEndScale(CableScale, false);

	SplineMesh->UpdateMesh();

	SplineMeshes.Add(SplineMesh);

	if (SplineMeshes.Num() < 2)
	{
		return;
	}

	// correct the location of the last spline
	int LastIndex = SplineMeshes.Num() - 2;
	SplineMeshes[LastIndex]->SetEndPosition(StartSplineLocation);
}

void ANerve::RemoveLastSplinePoint()
{
	int32 LastSplinePointIndex = SplineCable->GetNumberOfSplinePoints() - 1;
	SplineCable->RemoveSplinePoint(LastSplinePointIndex, true);

	int LastSplineMeshIndex = (SplineMeshes.Num() - 1);
	if (!SplineMeshes.IsValidIndex(LastSplineMeshIndex))
	{
		return;
	}

	USplineMeshComponent* LastSplineMesh = SplineMeshes[LastSplineMeshIndex];

	if (!LastSplineMesh)
	{
		return;
	}

	LastSplineMesh->DestroyComponent();

	SplineMeshes.RemoveAt(LastSplineMeshIndex);
}

void ANerve::UpdateLastSplinePointLocation(const FVector& NewLocation)
{
	int32 LastSplinePointIndex = SplineCable->GetNumberOfSplinePoints() - 1;
	SplineCable->SetLocationAtSplinePoint(LastSplinePointIndex, NewLocation, ESplineCoordinateSpace::World);

	int LastSplineMeshIndex = (SplineMeshes.Num() - 1);
	if (!SplineMeshes.IsValidIndex(LastSplineMeshIndex))
	{
		return;
	}

	USplineMeshComponent* LastSplineMesh = SplineMeshes[LastSplineMeshIndex];

	if (!LastSplineMesh)
	{
		return;
	}

	FVector RelativeNewLocation = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), NewLocation);
	LastSplineMesh->SetEndPosition(RelativeNewLocation, false);

	FVector SplineDirection = UKismetMathLibrary::GetDirectionUnitVector(LastSplineMesh->GetStartPosition(), RelativeNewLocation);
	LastSplineMesh->SetStartTangent(SplineDirection, false);
	LastSplineMesh->SetEndTangent(SplineDirection, false);

	LastSplineMesh->UpdateMesh();
}

void ANerve::ApplyCablesPhysics()
{
	if (!bShouldApplyCablePhysics)
	{
		return;
	}

	if (SplineCable->GetNumberOfSplinePoints() < 2)
	{
		return;
	}

	int32 LastPointIndex = SplineCable->GetNumberOfSplinePoints() - 2;

	FVector LastCableStartLocation = SplineCable->GetLocationAtSplinePoint(LastPointIndex, ESplineCoordinateSpace::World);
	FVector CableEndLocation = NerveBall->GetComponentLocation();

	UpdateLastSplinePointLocation(CableEndLocation);

	if (SplineCable->GetNumberOfSplineSegments() >= 2)
	{
		if (CanCurrentCableBeRemoved())
		{
			RemoveLastSplinePoint();
			return;
		}
	}

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	FHitResult Hit;

	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, LastCableStartLocation, CableEndLocation, CableColliders, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);

	if (!bHit)
	{
		return;
	}

	ImpactNormals.Add(Hit.Normal);

	FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(CableEndLocation, Hit.Location);

	bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, CableEndLocation, Hit.Location, CableColliders, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);
	if (!bHit)
	{
		return;
	}

	FVector WorldLocation = Hit.Location - (Direction * CableOffset);

	AddSplinePoint(WorldLocation, ESplineCoordinateSpace::World, true);
}

bool ANerve::CanCurrentCableBeRemoved()
{
	int32 CurrentPointIndex = SplineCable->GetNumberOfSplinePoints() - 1;
	int32 LastPointIndex = CurrentPointIndex - 1;

	FVector CurrentPointLocation = SplineCable->GetLocationAtSplinePoint(CurrentPointIndex, ESplineCoordinateSpace::World);
	FVector LastCableLocation = SplineCable->GetLocationAtSplinePoint(LastPointIndex, ESplineCoordinateSpace::World);

	FVector CurrentCableDirection = CurrentPointLocation - LastCableLocation;
	CurrentCableDirection.Normalize();

	int LastImpactNormalIndex = (ImpactNormals.Num() - 1);
	FVector LastImpactNormal = ImpactNormals[LastImpactNormalIndex];
	float DotResult = FVector::DotProduct((LastImpactNormal * -1), CurrentCableDirection);

	if (DotResult > 0.0f)
	{
		return false;
	}

	FHitResult Hit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	int32 OtherPointIndex = LastPointIndex - 1;
	FVector OtherPointLocation = SplineCable->GetLocationAtSplinePoint(OtherPointIndex, ESplineCoordinateSpace::World);
	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, OtherPointLocation, CurrentPointLocation, CableColliders, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);

	if (bHit)
	{
		return false;
	}

	ImpactNormals.RemoveAt(LastImpactNormalIndex);
	return true;
}

void ANerve::ResetCables()
{
	SplineCable->ClearSplinePoints(true);
	for (TObjectPtr<USplineMeshComponent> SplineMesh : SplineMeshes)
	{
		if (SplineMesh)
		{
			SplineMesh->DestroyComponent();
		}
	}

	SplineMeshes.Empty();

	ImpactNormals.Empty();

	FVector CableEndLocation = FVector(StartCableLength, 0.0f, 0.0f);

	AddSplinePoint(FVector::ZeroVector, ESplineCoordinateSpace::Local, false);
	AddSplinePoint(CableEndLocation, ESplineCoordinateSpace::Local, false);

	AddSplineMesh(FVector::ZeroVector, CableEndLocation, ESplineCoordinateSpace::Local);

	NerveBall->SetRelativeLocation(CableEndLocation);
}

void ANerve::RetractCable(float Alpha)
{
	float Distance = FMath::Lerp(SplineCable->GetDistanceAlongSplineAtSplinePoint(1), SplineCable->GetSplineLength(), Alpha);
	FVector TargetLocation = SplineCable->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);

	NerveBall->SetRelativeLocation(TargetLocation);

	int LastSplineMeshIndex = (SplineMeshes.Num() - 1);
	if (!SplineMeshes.IsValidIndex(LastSplineMeshIndex))
	{
		return;
	}

	USplineMeshComponent* LastSplineMesh = SplineMeshes[LastSplineMeshIndex];

	if (!LastSplineMesh)
	{
		return;
	}

	LastSplineMesh->SetEndPosition(TargetLocation, false);

	FVector SplineDirection = UKismetMathLibrary::GetDirectionUnitVector(LastSplineMesh->GetStartPosition(), TargetLocation);
	LastSplineMesh->SetStartTangent(SplineDirection, false);
	LastSplineMesh->SetEndTangent(SplineDirection, false);

	LastSplineMesh->UpdateMesh();

	float LastDistance = SplineCable->GetDistanceAlongSplineAtSplinePoint(RetractionIndex);

	if (Distance > LastDistance)
	{
		return;
	}

	RetractionIndex--;

	LastSplineMesh->DestroyComponent();

	SplineMeshes.RemoveAt(LastSplineMeshIndex);
}

void ANerve::FinishRetractCable()
{
	NerveBall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	InteractableComponent->AddInteractable(NerveBall);
	if (!InteractableComponent->OnInteract.IsAlreadyBound(this, &ANerve::Interaction))
	{
		InteractableComponent->OnInteract.AddDynamic(this, &ANerve::Interaction);
	}

	ResetCables();
}

FVector ANerve::GetLastCableLocation(const ESplineCoordinateSpace::Type& CoordinateSpace) const
{
	int32 LastIndex = (SplineCable->GetNumberOfSplinePoints() - 1);
	return SplineCable->GetLocationAtSplinePoint(LastIndex, CoordinateSpace);
}

float ANerve::GetCableLength() const
{
	return SplineCable->GetSplineLength();
}

FVector ANerve::GetCableDirection() const
{
	if (SplineCable->GetNumberOfSplinePoints() < 2)
	{
		return FVector::ZeroVector;
	}

	int32 CurrentPointIndex = SplineCable->GetNumberOfSplinePoints() - 1;
	int32 LastPointIndex = CurrentPointIndex - 1;

	FVector CurrentPointLocation = SplineCable->GetLocationAtSplinePoint(CurrentPointIndex, ESplineCoordinateSpace::World);
	FVector LastPointLocation = SplineCable->GetLocationAtSplinePoint(LastPointIndex, ESplineCoordinateSpace::World);

	return UKismetMathLibrary::GetDirectionUnitVector(CurrentPointLocation, LastPointLocation);
}

FVector ANerve::GetCablePosition(float Percent, ESplineCoordinateSpace::Type CoordinateSpace) const
{
	float Distance = FMath::Lerp(0.0f, SplineCable->GetSplineLength(), Percent);
	return SplineCable->GetLocationAtDistanceAlongSpline(Distance, CoordinateSpace);
}

#pragma endregion

#pragma region NerveBall

void ANerve::AttachNerveBall(AActor* ActorToAttach)
{
	NerveBall->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	bShouldApplyCablePhysics = true;

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	NerveBall->AttachToComponent(ActorToAttach->GetRootComponent(), Rules);
	NerveBall->SetRelativeLocation(GetDefault<UCharacterSettings>()->PawnGrabObjectOffset);
}

void ANerve::DetachNerveBall()
{
	PlayerController = nullptr;
	bShouldApplyCablePhysics = false;

	FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, true);
	NerveBall->AttachToComponent(RootComponent, Rules);

	SplineCable->AddSplinePointAtIndex(DefaultNervePosition, 1, ESplineCoordinateSpace::World, true);
	SplineCable->SetTangentAtSplinePoint(1, FVector::ZeroVector, ESplineCoordinateSpace::Local);

	RetractionIndex = SplineCable->GetNumberOfSplinePoints() - 2;

	float RetractionDuration = GetCableLength() / RetractionSpeed;
	RetractTimeline.SetPlayRate(1/RetractionDuration);

	RetractTimeline.ReverseFromEnd();
}

bool ANerve::IsNerveBallAttached() const
{
	return !NerveBall->IsAttachedTo(RootComponent);
}

#pragma endregion

#pragma region Interaction

void ANerve::Interaction(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent)
{
	if (CurrentAttachedReceptacle != nullptr)
	{
		CurrentAttachedReceptacle->TriggerLinkedObjects(this);
		CurrentAttachedReceptacle = nullptr;
	}

	PlayerController = Cast<AFirstPersonController>(Controller);
	AttachNerveBall(Pawn);

	PhysicConstraint = Cast<UPlayerToNervePhysicConstraint>(
		Pawn->AddComponentByClass(UPlayerToNervePhysicConstraint::StaticClass(), false, FTransform::Identity, false)
	);

	PhysicConstraint->Init(this, Cast<ACharacter>(Pawn));
	InteractableComponent->RemoveInteractable(NerveBall);
}

void ANerve::OnEnterWeakZone_Implementation(bool bIsZoneActive)
{
	IWeakZoneInterface::OnEnterWeakZone_Implementation(bIsZoneActive);

	if (bIsZoneActive && InteractableComponent->OnInteract.IsAlreadyBound(this, &ANerve::Interaction))
	{
		InteractableComponent->OnInteract.RemoveDynamic(this, &ANerve::Interaction);
	}
}

void ANerve::OnExitWeakZone_Implementation()
{
	IWeakZoneInterface::OnExitWeakZone_Implementation();

	if (!InteractableComponent->OnInteract.IsAlreadyBound(this, &ANerve::Interaction))
	{
		InteractableComponent->OnInteract.AddDynamic(this, &ANerve::Interaction);
	}
}

void ANerve::SetCurrentReceptacle(ANerveReceptacle* Receptacle)
{
	CurrentAttachedReceptacle = Receptacle;

	if (!CurrentAttachedReceptacle)
	{
		return;
	}

	bShouldApplyCablePhysics = false;
	FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);
	NerveBall->AttachToComponent(RootComponent, Rules);

	NerveBall->SetWorldLocation(Receptacle->GetActorLocation());
	UpdateLastSplinePointLocation(Receptacle->GetActorLocation());
	InteractableComponent->AddInteractable(NerveBall);
}

#pragma endregion
