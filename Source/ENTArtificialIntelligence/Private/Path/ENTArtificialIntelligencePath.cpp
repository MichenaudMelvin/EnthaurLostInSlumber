// Fill out your copyright notice in the Description page of Project Settings.


#include "Path/ENTArtificialIntelligencePath.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#endif

AENTArtificialIntelligencePath::AENTArtificialIntelligencePath()
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

void AENTArtificialIntelligencePath::BeginPlay()
{
	Super::BeginPlay();

	UpdatePoints(false);
}

void AENTArtificialIntelligencePath::OnConstruction(const FTransform& Transform)
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

void AENTArtificialIntelligencePath::UpdatePoints(bool bInConstructionScript)
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

		FVector TargetLocation = HitResult.Location;
		if (!IsOnFloor())
		{
			TargetLocation += HitResult.Normal * WallPointsOffset;
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

			ArrowComponent->SetWorldLocation(TargetLocation);
			ArrowComponent->SetWorldRotation(HitResult.Normal.Rotation());
			ArrowComponent->SetArrowColor(FLinearColor(0.0f, 0.2f, 0.8f));
			Arrows.Add(ArrowComponent);
		}
		else
#endif
		{
			Spline->SetLocationAtSplinePoint(i, TargetLocation, ESplineCoordinateSpace::World);
		}
	}

#if WITH_EDITORONLY_DATA
	if (!bInConstructionScript)
	{
		return;
	}

	for (int i = 0; i < Spline->GetNumberOfSplineSegments(); i++)
	{
		UActorComponent* Comp = AddComponentByClass(UArrowComponent::StaticClass(), true, FTransform::Identity, false);
		UArrowComponent* ArrowComponent = Cast<UArrowComponent>(Comp);
		if (!ArrowComponent)
		{
			continue;
		}

		FVector PointA = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		FVector PointB = Spline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);

		FVector SegmentDirection = UKismetMathLibrary::GetDirectionUnitVector(PointA, PointB);
		FRotator Rotation = FRotationMatrix::MakeFromX(SegmentDirection).Rotator();

		FVector MidPoint = (PointB + PointA) * 0.5f;
		ArrowComponent->SetWorldLocation(MidPoint);
		ArrowComponent->SetWorldRotation(Rotation);

		Arrows.Add(ArrowComponent);
	}
#endif
}

FVector AENTArtificialIntelligencePath::GetDirection() const
{
	FVector VectorDirection;

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

FVector AENTArtificialIntelligencePath::GetPointLocation(int8 PointIndex, float PawnHeight) const
{
	FVector PointLocation = Spline->GetLocationAtSplinePoint(PointIndex, ESplineCoordinateSpace::World);
	PointLocation += (GetDirection() * PawnHeight * -1);
	return PointLocation;
}

bool AENTArtificialIntelligencePath::GetTracedPointLocation(int8 PointIndex, FHitResult& HitResult)
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

bool AENTArtificialIntelligencePath::IsOnFloor() const
{
	return Direction == EAxis::Z && bInvertDirection;
}

#if WITH_EDITORONLY_DATA
bool AENTArtificialIntelligencePath::AttachAI(APawn* AI)
{
	if (!AI)
	{
		return false;
	}
	else if (AI == AttachedAI)
	{
		return true;
	}
	else if (AttachedAI)
	{
		return false;
	}

	AttachedAI = AI;
	return true;
}

void AENTArtificialIntelligencePath::DetachAI(APawn* AI)
{
	if (!AI)
	{
		return;
	}

	AttachedAI = nullptr;
}
#endif
