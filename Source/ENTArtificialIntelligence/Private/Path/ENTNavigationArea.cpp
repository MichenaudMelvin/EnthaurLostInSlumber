// Fill out your copyright notice in the Description page of Project Settings.


#include "Path/ENTNavigationArea.h"

#include "Components/SplineComponent.h"

#if WITH_EDITORONLY_DATA

#include "Components/BillboardComponent.h"
#include "Selection.h"
#include "Kismet/KismetSystemLibrary.h"

void FTriangle::DrawTriangle(const UObject* WorldContextObject, FLinearColor Color, float Thickness, float HeightOffset) const
{
	FVector OffsetZero = Zero;
	OffsetZero.Z += HeightOffset;
	FVector OffsetOne = One;
	OffsetOne.Z += HeightOffset;
	FVector OffsetTwo = Two;
	OffsetTwo.Z += HeightOffset;

	UKismetSystemLibrary::DrawDebugLine(WorldContextObject, OffsetZero, OffsetOne, Color, INFINITY, Thickness);
	UKismetSystemLibrary::DrawDebugLine(WorldContextObject, OffsetOne, OffsetTwo, Color, INFINITY, Thickness);
	UKismetSystemLibrary::DrawDebugLine(WorldContextObject, OffsetTwo, OffsetZero, Color, INFINITY, Thickness);
}
#endif

AENTNavigationArea::AENTNavigationArea()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Static);

	AreaSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	AreaSpline->SetupAttachment(Root);
	AreaSpline->SetMobility(EComponentMobility::Static);

	AreaSpline->ClearSplinePoints();
	AreaSpline->AddSplinePoint(FVector::ZeroVector, ESplineCoordinateSpace::Local, false);
	AreaSpline->AddSplinePoint(FVector(500.0f, 0.0f, 0.0f), ESplineCoordinateSpace::Local, false);
	AreaSpline->AddSplinePoint(FVector(0.0f, 500.0f, 0.0f), ESplineCoordinateSpace::Local, true);

#if WITH_EDITORONLY_DATA
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	BillboardComponent->SetupAttachment(Root);
	BillboardComponent->bIsEditorOnly = true;

	USelection::SelectObjectEvent.AddUObject(this, &AENTNavigationArea::OnSelectionUpdate);
	USelection::SelectionChangedEvent.AddUObject(this, &AENTNavigationArea::OnSelectionUpdate);
#endif
}

void AENTNavigationArea::BeginPlay()
{
	Super::BeginPlay();

	ComputeTriangles();
}

#if WITH_EDITORONLY_DATA
void AENTNavigationArea::Destroyed()
{
	Super::Destroyed();

	if (!SelectedInEditor)
	{
		return;
	}

	ClearTrianglesDraw();
}
#endif

void AENTNavigationArea::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetupSpline();

	ComputeTriangles();

#if WITH_EDITORONLY_DATA
	if (SelectedInEditor)
	{
		DrawTriangles();
	}
#endif
}

void AENTNavigationArea::SetupSpline() const
{
#if WITH_EDITORONLY_DATA
	AreaSpline->SetSelectedSplineSegmentColor(SplineColor);
	AreaSpline->SetUnselectedSplineSegmentColor(SplineColor);
#endif

	AreaSpline->SetClosedLoop(true, false);

	FVector SplineRelativeLocation = AreaSpline->GetRelativeLocation();
	SplineRelativeLocation.Z = SplineHeight;
	AreaSpline->SetRelativeLocation(SplineRelativeLocation);

	for (int i = 0; i < AreaSpline->GetNumberOfSplinePoints(); i++)
	{
		FVector FixedPointHeight = AreaSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FixedPointHeight.Z = 0.0f;

		AreaSpline->SetLocationAtSplinePoint(i, FixedPointHeight, ESplineCoordinateSpace::Local, false);
		AreaSpline->SetSplinePointType(i, ESplinePointType::Linear, false);
	}

	AreaSpline->UpdateSpline();
}

void AENTNavigationArea::ComputeTriangles()
{
	Triangles.Empty();
	TArray<FVector> SplinePointsLocations;

	for (int i = 0; i < AreaSpline->GetNumberOfSplinePoints(); i++)
	{
		SplinePointsLocations.AddUnique(AreaSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
	}

	float RightRotation = 0.0f;
	float LeftRotation = 0.0f;
	for (int i = 1; i < SplinePointsLocations.Num() - 1; i++)
	{
		FVector CurrentPoint = SplinePointsLocations[i];
		FVector NextPoint = SplinePointsLocations[i + 1];
		FVector PreviousPoint = SplinePointsLocations[i - 1];

		FVector ToNextPointUnit = (NextPoint - CurrentPoint).GetSafeNormal();
		FVector ToPreviousPointUnit = (PreviousPoint - CurrentPoint).GetSafeNormal();


		FRotator NextPointRotator = FRotationMatrix::MakeFromX(ToNextPointUnit).Rotator();
		FRotator PreviousPointRotator = FRotationMatrix::MakeFromX(ToPreviousPointUnit).Rotator();

		FRotator Delta = NextPointRotator - PreviousPointRotator;
		Delta.Normalize();

		float AbsYaw = FMath::Abs(Delta.Yaw);
		float RotationValue = Delta.Yaw > 180.0f ? AbsYaw : (360.0f - AbsYaw);

		if (FVector::CrossProduct(ToNextPointUnit, ToPreviousPointUnit).Z > 0.0f)
		{
			RightRotation += RotationValue;
		}
		else
		{
			LeftRotation += RotationValue;
		}
	}

	bool bRightRotation = RightRotation > LeftRotation;

	int32 IterationsCounter = 0;
	int32 Index = 0;
	while (SplinePointsLocations.Num() - 1 > 1 && SplinePointsLocations.Num() - 1 > IterationsCounter)
	{
		if (Index >= SplinePointsLocations.Num() - 1)
		{
			Index = 0;
		}

		FVector IndexZero = GetUbgrade(SplinePointsLocations, Index);
		FVector IndexOne = GetUbgrade(SplinePointsLocations, Index + 1);
		FVector IndexTwo = GetUbgrade(SplinePointsLocations, Index + 2);

		FTriangle NewTriangle;
		NewTriangle.Zero = IndexZero;
		NewTriangle.One = IndexOne;
		NewTriangle.Two = IndexTwo;

		bool bVal = FVector::CrossProduct((IndexZero - IndexOne), (IndexZero - IndexTwo)).Z > 0.0f;
		if (bRightRotation ? bVal : !bVal)
		{
			if (!FindPointInTriangle(SplinePointsLocations, NewTriangle))
			{
				Triangles.Add(NewTriangle);
				SplinePointsLocations.RemoveAt(Index + 1);
				IterationsCounter = 0;
			}
		}

		Index++;
		IterationsCounter++;
	}
}

FVector AENTNavigationArea::GetUbgrade(const TArray<FVector>& Vectors, int32 Index) const
{
	if (Vectors.IsValidIndex(Index))
	{
		return Vectors[Index];
	}

	return Vectors[Index - Vectors.Num()];
}

bool AENTNavigationArea::FindPointInTriangle(const TArray<FVector>& SplinePoints, FTriangle& Triangle) const
{
	TArray<FVector> PointsCopy = SplinePoints;

	PointsCopy.Remove(Triangle.Zero);
	PointsCopy.Remove(Triangle.One);
	PointsCopy.Remove(Triangle.Two);

	float TriangleSquare = FVector::CrossProduct((Triangle.Zero - Triangle.One), (Triangle.Zero - Triangle.Two)).Length() * 0.5f;

	for (FVector& Point : PointsCopy)
	{
		if (IsPointInTriangle(Triangle, Point, TriangleSquare))
		{
			return true;
		}
	}

	return false;
}

bool AENTNavigationArea::IsPointInTriangle(const FTriangle& Triangle, const FVector& Point, float TriangleSquare) const
{
	return TriangleSquare ==
		FVector::CrossProduct((Point - Triangle.One), (Point - Triangle.Two)).Length() * 0.5f +
		FVector::CrossProduct((Point - Triangle.Zero), (Point - Triangle.Two)).Length() * 0.5f +
		FVector::CrossProduct((Point - Triangle.Zero), (Point - Triangle.One)).Length() * 0.5f;
}

FVector AENTNavigationArea::GetRandomPointInArea() const
{
	const FTriangle& CurrentTriangle = Triangles[FMath::RandRange(0, Triangles.Num() - 1)];
	float RandomA = FMath::FRandRange(0.0f, 1.0f);
	float RandomB = FMath::FRandRange(0.0f, 1.0f - RandomA);

	FVector ResultA = (CurrentTriangle.One - CurrentTriangle.Zero) * RandomA;
	FVector ResultB = (CurrentTriangle.Two - CurrentTriangle.Zero) * RandomB;

	FVector RandomPoint = (CurrentTriangle.Zero + ResultA + ResultB);
	RandomPoint.Z -= SplineHeight;

	return RandomPoint;
}

#pragma region Debug

#if WITH_EDITORONLY_DATA
void AENTNavigationArea::OnSelectionUpdate(UObject* Object)
{
	if (Object == this && !SelectedInEditor)
	{
		SelectedInEditor = true;
		DrawTriangles();
	}
	else if (SelectedInEditor && !IsSelected())
	{
		SelectedInEditor = false;
		ClearTrianglesDraw();
	}
}

void AENTNavigationArea::DrawTriangles() const
{
	ClearTrianglesDraw();

	if (!bDrawTriangles)
	{
		return;
	}

	for (const FTriangle& Triangle : Triangles)
	{
		Triangle.DrawTriangle(this, TriangleColor, TriangleThickness, bDrawWithOffset ? (-SplineHeight + TriangleThickness) : 0.0f);
	}
}

void AENTNavigationArea::ClearTrianglesDraw() const
{
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
}

void AENTNavigationArea::DrawRandomPoint() const
{
	DrawTriangles();

	UKismetSystemLibrary::DrawDebugPoint(this, GetRandomPointInArea(), 25.0f, PointColor, INFINITY);
}

#endif

#pragma endregion