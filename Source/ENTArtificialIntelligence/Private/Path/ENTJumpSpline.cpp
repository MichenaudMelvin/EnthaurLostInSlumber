// Fill out your copyright notice in the Description page of Project Settings.


#include "Path/ENTJumpSpline.h"

#include "Components/SplineComponent.h"


AENTJumpSpline::AENTJumpSpline()
{
	PrimaryActorTick.bCanEverTick = false;
	JumpSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(JumpSpline);
}

void AENTJumpSpline::InitSpline(const FTransform& StartTransform, const FTransform& EndTransform) const
{
	float DistanceFactor = 0.5f;

	JumpSpline->ClearSplinePoints(false);

	JumpSpline->AddSplinePoint(StartTransform.GetLocation(), ESplineCoordinateSpace::World, false);

	FVector MiddlePoint = (((EndTransform.GetLocation() - StartTransform.GetLocation()) * 0.5f) + StartTransform.GetLocation());
	float Distance = FVector::Dist(StartTransform.GetLocation(), EndTransform.GetLocation()) * DistanceFactor;

	MiddlePoint.Z += Distance;

	JumpSpline->AddSplinePoint(MiddlePoint, ESplineCoordinateSpace::World, false);

	JumpSpline->AddSplinePoint(EndTransform.GetLocation(), ESplineCoordinateSpace::World, false);

	JumpSpline->UpdateSpline();
}

FVector AENTJumpSpline::GetLocationAtAlpha(float Alpha) const
{
	float Distance = FMath::Lerp(0.0f, JumpSpline->GetSplineLength(), Alpha);

	return JumpSpline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
}

#if WITH_EDITORONLY_DATA
void AENTJumpSpline::ShowSpline(const FLinearColor& SplineColor) const
{
	JumpSpline->SetSelectedSplineSegmentColor(SplineColor);
	JumpSpline->SetUnselectedSplineSegmentColor(SplineColor);

	JumpSpline->SetHiddenInGame(false);
	JumpSpline->SetVisibility(true);
}
#endif
