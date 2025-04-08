// Fill out your copyright notice in the Description page of Project Settings.


#include "Path/AIPath.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#if WITH_EDITORONLY_DATA
#include "Components/BillboardComponent.h"
#endif

AAIPath::AAIPath()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	Spline->SetupAttachment(Root);
	Spline->SetClosedLoop(true);

#if WITH_EDITORONLY_DATA
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>("Billboard");
	BillboardComponent->SetupAttachment(Root);
#endif

	GroundObjectTypes.Add(ObjectTypeQuery1);
	GroundObjectTypes.Add(ObjectTypeQuery2);
}

void AAIPath::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITORONLY_DATA
	UpdatePoints(false);
#endif
}

void AAIPath::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector SplineRelativeLocation = Spline->GetRelativeLocation();
	SplineRelativeLocation.Z = SplineHeight;
	Spline->SetRelativeLocation(SplineRelativeLocation);

	UpdatePoints(true);

#if WITH_EDITORONLY_DATA
	if (AttachedAI)
	{
		AttachedAI->OnConstruction(AttachedAI->GetActorTransform());
	}
#endif
}

void AAIPath::UpdatePoints(bool bInConstructionScript)
{
#if WITH_EDITORONLY_DATA
	Arrows.Empty();
#endif

	for (int i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
	{
		Spline->SetTangentsAtSplinePoint(i, FVector::ZeroVector, FVector::ZeroVector, ESplineCoordinateSpace::World, true);

		FHitResult HitResult;
		bool bHit = GetTracedPointLocation(i, HitResult);

		if (!bHit)
		{
			continue;
		}

#if WITH_EDITORONLY_DATA
		if (bInConstructionScript)
		{
			UActorComponent* Comp = AddComponentByClass(UArrowComponent::StaticClass(), true, FTransform::Identity, false);
			UArrowComponent* ArrowComponent = Cast<UArrowComponent>(Comp);
			if (!ArrowComponent)
			{
				continue;
			}

			ArrowComponent->SetWorldLocation(HitResult.Location);
			ArrowComponent->SetWorldRotation(HitResult.Normal.Rotation());
			Arrows.Add(ArrowComponent);
		}
		else
#endif
		{
			Spline->SetLocationAtSplinePoint(i, HitResult.Location, ESplineCoordinateSpace::World);
		}
	}
}

FVector AAIPath::GetDirection()
{
	FVector VectorDirection;

	if (bAutoDirection)
	{
		Direction;
	}

	switch (Direction)
	{
	default:
	case EAxis::None:
		VectorDirection = FVector::ZeroVector;
		break;
	case EAxis::X:
		VectorDirection = FVector::ForwardVector;
		break;
	case EAxis::Y:
		VectorDirection = FVector::RightVector;
		break;
	case EAxis::Z:
		VectorDirection = FVector::UpVector;
		break;
	}

	VectorDirection *= (bInvertDirection ? -1 : 1);

	return VectorDirection;
}

FVector AAIPath::GetPointLocation(int8 PointIndex) const
{
	return Spline->GetLocationAtSplinePoint(PointIndex, ESplineCoordinateSpace::World);
}

bool AAIPath::GetTracedPointLocation(int8 PointIndex, FHitResult& HitResult)
{
	FVector TraceDirection = GetDirection();

	TraceDirection *= (SplineHeight + TraceLength);

	FVector PointLocation = Spline->GetLocationAtSplinePoint(PointIndex, ESplineCoordinateSpace::World);
	FVector EndLocation = PointLocation;
	EndLocation += TraceDirection;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	return UKismetSystemLibrary::LineTraceSingleForObjects(this, PointLocation, EndLocation, GroundObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, false);
}

bool AAIPath::IsOnFloor() const
{
	return Direction == EAxis::Z && bInvertDirection;
}

#if WITH_EDITORONLY_DATA
bool AAIPath::AttachAI(APawn* AI)
{
	if (!AI || AttachedAI)
	{
		return false;
	}

	AttachedAI = AI;
	return true;
}

void AAIPath::DetachAI(APawn* AI)
{
	if (!AI)
	{
		return;
	}

	AttachedAI = nullptr;
}
#endif
