// Fill out your copyright notice in the Description page of Project Settings.


#include "Path/ENTArtificialIntelligencePath.h"

#include "AIController.h"
#include "NavLinkCustomComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

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

	PathLink = CreateDefaultSubobject<UNavLinkCustomComponent>(TEXT("StartNavLink"));
	PathLink->SetMoveReachedLink(this, &AENTArtificialIntelligencePath::NotifyLinkReached);
	PathLink->SetNavigationRelevancy(true);

#if WITH_EDITORONLY_DATA
	DebugMeshRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("DebugRoot"));
	DebugMeshRootComp->SetupAttachment(Root);
	DebugMeshRootComp->SetMobility(EComponentMobility::Static);
	DebugMeshRootComp->bIsEditorOnly = true;

	DebugMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("DebugMesh");
	DebugMeshComp->SetupAttachment(DebugMeshRootComp);
	DebugMeshComp->bIsEditorOnly = true;
	DebugMeshComp->SetMobility(EComponentMobility::Static);
	DebugMeshComp->CastShadow = false;
	DebugMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	DebugMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugMeshComp->SetCanEverAffectNavigation(false);

	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>("Billboard");
	BillboardComponent->SetupAttachment(Root);
	BillboardComponent->bIsEditorOnly = true;
#endif

	GroundObjectTypes.Add(ObjectTypeQuery1);
	GroundObjectTypes.Add(ObjectTypeQuery2);
}

void AENTArtificialIntelligencePath::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITORONLY_DATA
	DebugMeshComp->DestroyComponent();
#endif

	if (bIsAClosedLoop || IsOnFloor())
	{
		PathLink->DestroyComponent();
	}

	UpdatePoints(false);
}

void AENTArtificialIntelligencePath::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector SplineRelativeLocation = Spline->GetRelativeLocation();
	SplineRelativeLocation.Z = SplineHeight;
	Spline->SetRelativeLocation(SplineRelativeLocation);
	Spline->SetClosedLoop(bIsAClosedLoop);

	UpdatePoints(true);

	PathLink->SetLinkData(StartNavLinkLocation, EndNavLinkLocation, ENavLinkDirection::BothWays);

#if WITH_EDITORONLY_DATA
	if (AttachedAI)
	{
		AttachedAI->OnConstruction(AttachedAI->GetActorTransform());
	}

	TArray<TObjectPtr<AENTArtificialIntelligencePath>> PathsToUpdate = PreviousPaths;
	PreviousPaths.Empty();
	for (TObjectPtr<AENTArtificialIntelligencePath> PathToUpdate : PathsToUpdate)
	{
		if (PathToUpdate)
		{
			PathToUpdate->UpdateNavLink();
		}
	}
#endif

	UpdateNavLink();
}

#if WITH_EDITOR
void AENTArtificialIntelligencePath::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.GetPropertyName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AENTArtificialIntelligencePath, NextPath))
	{
		if (NextPath)
		{
			if (NextPath == this || bIsAClosedLoop)
			{
				NextPath->PreviousPaths.Remove(this);
				NextPath = nullptr;
			}
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(AENTArtificialIntelligencePath, bIsAClosedLoop))
	{
		if (bIsAClosedLoop && NextPath)
		{
			NextPath->PreviousPaths.Remove(this);
			NextPath = nullptr;
		}
	}
}
#endif

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
			ArrowComponent->SetArrowSize(GroundArrowSize);
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
		ArrowComponent->SetArrowSize(PathArrowSize);

		Arrows.Add(ArrowComponent);
	}

	DebugMeshComp->SetStaticMesh(DebugMesh);

	if (!bShowDebugMesh)
	{
		DebugMeshComp->SetVisibility(false);
		return;
	}

	DebugMeshComp->SetVisibility(true);

	DebugMeshRootComp->SetWorldTransform(GetTransformAtAlpha(DebugSplineAlpha));
	DebugMeshComp->SetRelativeRotation(RotationOffset);
#endif
}

void AENTArtificialIntelligencePath::UpdateNavLink()
{
	if (bIsAClosedLoop || !NextPath || NextPath == this)
	{
		return;
	}

	FTransform FirstTransform = NextPath->GetFirstPointTransform(ESplineCoordinateSpace::World);
	FVector LocalPosition = GetActorTransform().InverseTransformPosition(FirstTransform.GetLocation());

	FTransform LastTransform = GetLastPointTransform(ESplineCoordinateSpace::Local);

#if WITH_EDITORONLY_DATA
	if (!NextPath->PreviousPaths.Contains(this))
	{
		NextPath->PreviousPaths.Add(this);
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

FVector AENTArtificialIntelligencePath::GetPointLocation(int32 PointIndex, float PawnHeight) const
{
	FVector PointLocation = Spline->GetLocationAtSplinePoint(PointIndex, ESplineCoordinateSpace::World);
	PointLocation += (GetDirection() * PawnHeight * -1);
	return PointLocation;
}

FTransform AENTArtificialIntelligencePath::GetFirstPointTransform(const ESplineCoordinateSpace::Type& CoordinateSpace) const
{
	return Spline->GetTransformAtSplinePoint(0, CoordinateSpace);
}

FTransform AENTArtificialIntelligencePath::GetLastPointTransform(const ESplineCoordinateSpace::Type& CoordinateSpace) const
{
	int32 Index = Spline->GetNumberOfSplinePoints() - 1;
	return Spline->GetTransformAtSplinePoint(Index, CoordinateSpace);
}

FTransform AENTArtificialIntelligencePath::GetTransformAtAlpha(float Alpha) const
{
	float Distance = FMath::Lerp(0.0f, Spline->GetSplineLength(), Alpha);
	FTransform TargetTransform = Spline->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

	float NextAlpha = FMath::Clamp(Alpha + 0.1f, 0.0f, 1.0f);
	float NextDistance = FMath::Lerp(0.0f, Spline->GetSplineLength(), NextAlpha);
	FVector NextLocation = Spline->GetLocationAtDistanceAlongSpline(NextDistance, ESplineCoordinateSpace::World);

	FVector ForwardDirection = UKismetMathLibrary::GetDirectionUnitVector(TargetTransform.GetLocation(), NextLocation);
	FQuat TargetRotation = FRotationMatrix::MakeFromXZ(ForwardDirection, (GetDirection() * -1)).ToQuat();

	TargetTransform.SetRotation(TargetRotation);

	return TargetTransform;
}

FVector AENTArtificialIntelligencePath::GetNavLinkLocation(int32 PathDirection) const
{
	FVector StartPoint;
	FVector EndPoint;
	ENavLinkDirection::Type NavLinkDirection;

	PathLink->GetLinkData(StartPoint, EndPoint, NavLinkDirection);
	return PathDirection == 1 ? (EndPoint + GetActorLocation()) : (StartPoint + GetActorLocation());
}

bool AENTArtificialIntelligencePath::IsAtTheEndOfThePath(uint16 Index, int32 PathDirection) const
{
	return PathDirection == 1 ? Index == (Spline->GetNumberOfSplinePoints() - 1) : Index == 0;
}

bool AENTArtificialIntelligencePath::IsAtTheEndOfThePath(const FVector& ActorLocation, int32 PathDirection, float Tolerance) const
{
	FVector StartLocation = GetStartTransform().GetLocation();
	FVector EndLocation = GetEndTransform().GetLocation();

	StartLocation += GetDirection() * -1 * SplineHeight;
	EndLocation += GetDirection() * -1 * SplineHeight;

	if (PathDirection == -1 && ActorLocation.Equals(StartLocation, Tolerance))
	{
		return true;
	}
	else if (PathDirection == 1 && ActorLocation.Equals(EndLocation, Tolerance))
	{
		return true;
	}
	else
	{
		return false;
	}
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

#pragma region NavLinks

void AENTArtificialIntelligencePath::NotifyLinkReached(UNavLinkCustomComponent* NavLinkCustomComponent, UObject* PathingAgent, const UE::Math::TVector<double>& Destination)
{
	UPathFollowingComponent* PathComp = Cast<UPathFollowingComponent>(PathingAgent);
	if (!PathComp)
	{
		return;
	}

	AActor* PathOwner = PathComp->GetOwner();
	if (!PathOwner)
	{
		return;
	}

	AAIController* ControllerOwner = Cast<AAIController>(PathOwner);
	if (!ControllerOwner)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = ControllerOwner->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	FVector2D WorldStartPoint = FVector2D(StartNavLinkLocation + GetActorLocation());
	FVector2D WorldEndPoint = FVector2D(EndNavLinkLocation + GetActorLocation());

	if (FVector2D(Destination) == WorldStartPoint)
	{
		BlackboardComp->SetValueAsInt(PathIndexKeyName, Spline->GetNumberOfSplinePoints() - 1);
		BlackboardComp->SetValueAsInt(PathDirectionKeyName, -1);
		BlackboardComp->SetValueAsVector(NextPathLocationKeyName, GetEndTransform().GetLocation());
	}
	else if (FVector2D(Destination) == WorldEndPoint)
	{
		BlackboardComp->SetValueAsInt(PathIndexKeyName, 0);
		BlackboardComp->SetValueAsInt(PathDirectionKeyName, 1);
		BlackboardComp->SetValueAsVector(NextPathLocationKeyName, GetStartTransform().GetLocation());
	}
	else
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Failed to find destination"));

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	BlackboardComp->SetValueAsBool(JumpKeyName, true);
	BlackboardComp->SetValueAsObject(AIPathKeyName, this);
}

#pragma endregion

#pragma region Debug

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

#pragma endregion