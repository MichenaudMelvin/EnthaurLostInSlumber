// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ENTNavigationArea.generated.h"

class USplineComponent;

USTRUCT(BlueprintType)
struct FTriangle
{
	GENERATED_BODY()

	FVector Zero = FVector::ZeroVector;
	FVector One = FVector::ZeroVector;
	FVector Two = FVector::ZeroVector;

#if WITH_EDITORONLY_DATA
	void DrawTriangle(const UObject* WorldContextObject, FLinearColor Color = FLinearColor::Red, float Thickness = 1.0f, float HeightOffset = 0.0f) const;
#endif
};

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API AENTNavigationArea : public AActor
{
	GENERATED_BODY()

public:
	AENTNavigationArea();

protected:
	virtual void BeginPlay() override;

#if WITH_EDITORONLY_DATA
	virtual void Destroyed() override;
#endif

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Area")
	TObjectPtr<USplineComponent> AreaSpline;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	TObjectPtr<UBillboardComponent> BillboardComponent;
#endif

	UPROPERTY(EditDefaultsOnly, Category = "Area", meta = (Units = cm, ClampMin = 0.0f))
	float SplineHeight = 100.0f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Area")
	FLinearColor SplineColor = FLinearColor::Blue;
#endif

	void SetupSpline() const;

	UPROPERTY(VisibleInstanceOnly, Category = "Triangles")
	TArray<FTriangle> Triangles;

	void ComputeTriangles();

	FVector GetUbgrade(const TArray<FVector>& Vectors, int32 Index) const;

	bool FindPointInTriangle(const TArray<FVector>& SplinePoints, FTriangle& Triangle) const;

	bool IsPointInTriangle(const FTriangle& Triangle, const FVector& Point, float TriangleSquare) const;

public:
	// https://github.com/SoulofAO/Random_Points_In_Spline_Zone_UE4Plugin
	UFUNCTION(BlueprintCallable, Category = "Area")
	FVector GetRandomPointInArea() const;

#pragma region Debug

#if WITH_EDITOR

protected:
	void OnSelectionUpdate(UObject* Object);

	void DrawTriangles() const;

	void ClearTrianglesDraw() const;

	UFUNCTION(CallInEditor, Category = "Debug")
	void DrawRandomPoint() const;

#endif

#if WITH_EDITORONLY_DATA

	bool SelectedInEditor = false;

	UPROPERTY(EditAnywhere, Category = "Debug")
	FLinearColor TriangleColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, Category = "Debug")
	FLinearColor PointColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, Category = "Debug")
	float TriangleThickness = 10.0f;

	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDrawWithOffset = false;

	UPROPERTY(EditInstanceOnly, Transient, Category = "Debug")
	bool bDrawTriangles = true;

#endif

#pragma endregion

};
