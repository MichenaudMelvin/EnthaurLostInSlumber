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

	FirstNavLink = CreateDefaultSubobject<UNavLinkCustomComponent>(TEXT("FirstNavLink"));
	FirstNavLink->SetMoveReachedLink(this, &AENTArtificialIntelligencePath::NotifyLinkReached);
	FirstNavLink->SetNavigationRelevancy(true);

	SecondNavLink = CreateDefaultSubobject<UNavLinkCustomComponent>(TEXT("SecondNavLink"));
	SecondNavLink->SetMoveReachedLink(this, &AENTArtificialIntelligencePath::NotifyLinkReached);
	SecondNavLink->SetNavigationRelevancy(true);

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

	NavLinkPlatform = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NavLinkPlatform"));
	NavLinkPlatform->SetupAttachment(Root);
	NavLinkPlatform->SetMobility(EComponentMobility::Static);
	NavLinkPlatform->SetCollisionResponseToAllChannels(ECR_Ignore);
	NavLinkPlatform->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	NavLinkPlatform->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	NavLinkPlatform->SetHiddenInGame(true);
	NavLinkPlatform->CastShadow = false;
	NavLinkPlatform->bIsEditorOnly = true;

	FistNavLinkDebugArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FistNavLinkDebugArrow"));
	FistNavLinkDebugArrow->SetupAttachment(Root);
	FistNavLinkDebugArrow->SetWorldRotation(FRotator(90.0f, 0.0f, 0.0f));

	SecondNavLinkDebugArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("SecondNavLinkDebugArrow"));
	SecondNavLinkDebugArrow->SetupAttachment(Root);
	SecondNavLinkDebugArrow->SetWorldRotation(FRotator(90.0f, 0.0f, 0.0f));

	FistNavLinkPlatformDebugArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FistNavLinkPlatformDebugArrow"));
	FistNavLinkPlatformDebugArrow->SetupAttachment(Root);
	FistNavLinkPlatformDebugArrow->SetArrowColor(FLinearColor::Green);

	SecondNavLinkPlatformDebugArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("SecondNavLinkPlatformDebugArrow"));
	SecondNavLinkPlatformDebugArrow->SetupAttachment(Root);
	SecondNavLinkPlatformDebugArrow->SetArrowColor(FLinearColor::Green);
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
		FirstNavLink->DestroyComponent();
		SecondNavLink->DestroyComponent();
	}

#if WITH_EDITORONLY_DATA
	if (NavLinkPlatform)
	{
		NavLinkPlatform->DestroyComponent();
	}
#endif

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

	if (NavLinksPointAxis == EAxis::Z)
	{
		NavLinksPointAxis = EAxis::X;
	}

	FistNavLinkDebugArrow->SetRelativeLocation(FirstNavLinkLocation);
	SecondNavLinkDebugArrow->SetRelativeLocation(SecondNavLinkLocation);

	BuildNavLinkPlatform();
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

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AENTArtificialIntelligencePath, FirstNavLinkLocation) || PropertyName == GET_MEMBER_NAME_CHECKED(AENTArtificialIntelligencePath, bIgnoreGroundTrace))
	{
		if (!bIgnoreGroundTrace)
		{
			FVector StartLocation = FirstNavLinkLocation + GetActorLocation();
			StartLocation.Z += GroundTraceLength;
			FVector EndLocation = FirstNavLinkLocation + GetActorLocation();
			EndLocation.Z -= GroundTraceLength;

			TArray<AActor*> ActorToIgnore;
			FHitResult HitResult;

			bool bHit = UKismetSystemLibrary::LineTraceSingle(this, StartLocation, EndLocation, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorToIgnore, EDrawDebugTrace::None, HitResult, true);

			if (bHit)
			{
				FirstNavLinkLocation = HitResult.Location - GetActorLocation();
			}

		}

		FistNavLinkDebugArrow->SetRelativeLocation(FirstNavLinkLocation);
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AENTArtificialIntelligencePath, SecondNavLinkLocation) || PropertyName == GET_MEMBER_NAME_CHECKED(AENTArtificialIntelligencePath, bIgnoreGroundTrace))
	{
		if (!bIgnoreGroundTrace)
		{
			FVector StartLocation = SecondNavLinkLocation + GetActorLocation();
			StartLocation.Z += GroundTraceLength;
			FVector EndLocation = SecondNavLinkLocation + GetActorLocation();
			EndLocation.Z -= GroundTraceLength;

			TArray<AActor*> ActorToIgnore;
			FHitResult HitResult;

			bool bHit = UKismetSystemLibrary::LineTraceSingle(this, StartLocation, EndLocation, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorToIgnore, EDrawDebugTrace::None, HitResult, true);

			if (bHit)
			{
				SecondNavLinkLocation = HitResult.Location - GetActorLocation();
			}
		}

		SecondNavLinkDebugArrow->SetRelativeLocation(SecondNavLinkLocation);
	}
}
#endif

void AENTArtificialIntelligencePath::UpdatePoints(bool bInConstructionScript)
{
#if WITH_EDITORONLY_DATA
	Arrows.Empty();
#endif

	for (int32 i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
	{
		Spline->SetTangentsAtSplinePoint(i, FVector::ZeroVector, FVector::ZeroVector, ESplineCoordinateSpace::World, true);

		FHitResult HitResult;
		bool bHit = GetTracedPointLocation(i, HitResult);

		FVector TargetLocation = bHit ? HitResult.Location : Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
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

	DebugMeshRootComp->SetWorldTransform(GetTransformAtAlpha(DebugSplineAlpha, (DebugInvertMeshDirection ? -1 : 1)));
	DebugMeshComp->SetRelativeRotation(RotationOffset);
#endif
}

void AENTArtificialIntelligencePath::UpdateNavLink()
{
	if (bIsAClosedLoop || !NextPath || NextPath == this)
	{
		return;
	}

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

FTransform AENTArtificialIntelligencePath::GetTransformAtAlpha(float Alpha, int8 SplineDirection) const
{
	float Distance = FMath::Lerp(0.0f, Spline->GetSplineLength(), Alpha);
	FTransform TargetTransform = Spline->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

	float NextAlpha = FMath::Clamp(Alpha + (0.1f * SplineDirection), 0.0f, 1.0f);
	float NextDistance = FMath::Lerp(0.0f, Spline->GetSplineLength(), NextAlpha);
	FVector NextLocation = Spline->GetLocationAtDistanceAlongSpline(NextDistance, ESplineCoordinateSpace::World);

	FVector ForwardDirection = UKismetMathLibrary::GetDirectionUnitVector(TargetTransform.GetLocation(), NextLocation);
	FQuat TargetRotation = FRotationMatrix::MakeFromXZ(ForwardDirection, (GetDirection() * -1)).ToQuat();

	TargetTransform.SetRotation(TargetRotation);

	return TargetTransform;
}

FVector AENTArtificialIntelligencePath::GetNavLinkLocation(int32 PathDirection) const
{
	return (PathDirection == 1 ? SecondNavLinkLocation : FirstNavLinkLocation) + GetActorLocation();
}

bool AENTArtificialIntelligencePath::IsAtTheEndOfThePath(uint16 Index, int32 PathDirection) const
{
	return PathDirection == 1 ? Index == (Spline->GetNumberOfSplinePoints() - 1) : Index == 0;
}

bool AENTArtificialIntelligencePath::IsAtTheEndOfThePath(const FVector& ActorLocation, float ActorHeight, int32 PathDirection, float Tolerance) const
{
	FVector StartLocation = GetStartTransform(PathDirection).GetLocation();
	FVector EndLocation = GetEndTransform(PathDirection).GetLocation();

	StartLocation += GetDirection() * -1;
	EndLocation += GetDirection() * -1;

	FVector ActorLocationCopy = ActorLocation;
	ActorLocationCopy += GetDirection() * ActorHeight;

#if WITH_EDITORONLY_DATA
	if (bShowTraces)
	{
		UKismetSystemLibrary::DrawDebugPoint(this, StartLocation, 5.0f, FLinearColor::Blue, 0.0f);
		UKismetSystemLibrary::DrawDebugPoint(this, EndLocation, 5.0f, FLinearColor::Blue, 0.0f);

		UKismetSystemLibrary::DrawDebugPoint(this, ActorLocationCopy, 5.0f, FLinearColor::Yellow, 0.0f);
	}
#endif

	if (PathDirection == -1 && ActorLocationCopy.Equals(StartLocation, Tolerance))
	{
		return true;
	}
	else if (PathDirection == 1 && ActorLocationCopy.Equals(EndLocation, Tolerance))
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
	if (!NavLinkCustomComponent)
	{
		return;
	}

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

	if (NavLinkCustomComponent == FirstNavLink)
	{
		BlackboardComp->SetValueAsInt(PathIndexKeyName, 1);
		BlackboardComp->SetValueAsInt(PathDirectionKeyName, 1);
		BlackboardComp->SetValueAsVector(JumpLocationKeyName, GetStartTransform(1).GetLocation());
	}
	else if (NavLinkCustomComponent == SecondNavLink)
	{
		BlackboardComp->SetValueAsInt(PathIndexKeyName, Spline->GetNumberOfSplinePoints() - 2);
		BlackboardComp->SetValueAsInt(PathDirectionKeyName, -1);
		BlackboardComp->SetValueAsVector(JumpLocationKeyName, GetEndTransform(-1).GetLocation());
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

#if WITH_EDITORONLY_DATA
void AENTArtificialIntelligencePath::BuildNavLinkPlatform() const
{
	if (!NavLinkPlatform)
	{
		return;
	}

	if (!bUsePlatform || bIsAClosedLoop || IsOnFloor())
	{
		FirstNavLink->SetLinkData(FirstNavLinkLocation, SecondNavLinkLocation, ENavLinkDirection::BothWays);
		SecondNavLink->SetLinkData(SecondNavLinkLocation, FirstNavLinkLocation, ENavLinkDirection::BothWays);

		NavLinkPlatform->SetCanEverAffectNavigation(false);
		NavLinkPlatform->SetVisibility(false);
		FistNavLinkPlatformDebugArrow->SetVisibility(false);
		SecondNavLinkPlatformDebugArrow->SetVisibility(false);
		return;
	}

	if (!NavLinkPlatform->GetStaticMesh())
	{
		return;
	}

	NavLinkPlatform->SetCanEverAffectNavigation(true);
	NavLinkPlatform->SetVisibility(bShowPlatform);
	FistNavLinkPlatformDebugArrow->SetVisibility(true);
	SecondNavLinkPlatformDebugArrow->SetVisibility(true);

	FVector MeshSize = (NavLinkPlatform->GetStaticMesh()->GetBoundingBox().Max - NavLinkPlatform->GetStaticMesh()->GetBoundingBox().Min);

	FVector FirstPoint = Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local) + PlatformOffset;
	FVector SecondPoint = Spline->GetLocationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local) + PlatformOffset;

	FVector Offset = (GetDirection() * -1) * (PlatformScale * (MeshSize.Y * 0.5f));
	FVector OffsetFirstPoint = FirstPoint + Offset;
	FVector OffsetSecondPoint = SecondPoint + Offset;

	NavLinkPlatform->SetRelativeLocation(OffsetFirstPoint);

	FVector FirstNavLinkEndLocation;
	FVector SecondNavLinkEndLocation;

	if (bPointPlatform)
	{
		NavLinkPlatform->SetWorldRotation(FRotator::ZeroRotator);
		NavLinkPlatform->SetWorldScale3D(FVector(OverridenNavLinkScale.X, OverridenNavLinkScale.Y, 1.0f));

		FirstNavLinkEndLocation = NavLinkPlatform->GetRelativeLocation();
		SecondNavLinkEndLocation = NavLinkPlatform->GetRelativeLocation();

		if (NavLinksPointAxis == EAxis::X)
		{
			float PointOffset = MeshSize.X * OverridenNavLinkScale.X * 0.5f;

			FirstNavLinkEndLocation.X += PointOffset + NavLinkOffset;
			SecondNavLinkEndLocation.X += PointOffset - NavLinkOffset;
		}
		else if (NavLinksPointAxis == EAxis::Y)
		{
			float PointOffset = (OverridenNavLinkScale.Y * 0.5f);

			FirstNavLinkEndLocation.X += MeshSize.X * OverridenNavLinkScale.X * 0.5f;
			SecondNavLinkEndLocation.X += MeshSize.X * OverridenNavLinkScale.X * 0.5f;

			FirstNavLinkEndLocation.Y += PointOffset + NavLinkOffset;
			SecondNavLinkEndLocation.Y -= PointOffset + NavLinkOffset;
		}
	}
	else
	{
		float Distance = FVector::Dist(FirstPoint, SecondPoint);

		NavLinkPlatform->SetRelativeRotation(UKismetMathLibrary::FindLookAtRotation(FirstPoint, SecondPoint));
		NavLinkPlatform->SetWorldScale3D(FVector(Distance / MeshSize.X, PlatformScale, 1.0f));

		FVector NavLinkPointOffset =(NavLinkPlatform->GetForwardVector() * NavLinkOffset);
		FirstNavLinkEndLocation = OffsetFirstPoint + NavLinkPointOffset;
		SecondNavLinkEndLocation = OffsetSecondPoint - NavLinkPointOffset;
	}

	FirstNavLink->SetLinkData(FirstNavLinkLocation, FirstNavLinkEndLocation, ENavLinkDirection::BothWays);
	SecondNavLink->SetLinkData(SecondNavLinkLocation, SecondNavLinkEndLocation, ENavLinkDirection::BothWays);

	FistNavLinkPlatformDebugArrow->SetRelativeLocation(FirstNavLinkEndLocation);
	SecondNavLinkPlatformDebugArrow->SetRelativeLocation(SecondNavLinkEndLocation);

	FistNavLinkPlatformDebugArrow->SetRelativeRotation(NavLinkPlatform->GetRelativeRotation() + FRotator(90.0f, 0.0f, 0.0f));
	SecondNavLinkPlatformDebugArrow->SetRelativeRotation(NavLinkPlatform->GetRelativeRotation() + FRotator(90.0f, 0.0f, 0.0f));
}
#endif

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