// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ENTArtificialIntelligencePath.generated.h"

namespace ESplineCoordinateSpace
{
	enum Type : int;
}

class UNavLinkComponent;
class USplineComponent;

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API AENTArtificialIntelligencePath : public AActor
{
	GENERATED_BODY()

public:
	AENTArtificialIntelligencePath();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNavLinkComponent> LinkToAnotherPath;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	TObjectPtr<UBillboardComponent> BillboardComponent;

	UPROPERTY(Transient)
	TArray<TObjectPtr<class UArrowComponent>> Arrows;

	UPROPERTY(EditAnywhere, Category = "Debug", meta = (ClampMin = 0.0f))
	float GroundArrowSize = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Debug", meta = (ClampMin = 0.0f))
	float PathArrowSize = 2.5f;
#endif

	/**
	 * @brief Only work if the spline is not a closed loop
	 */
	UPROPERTY(EditInstanceOnly, Category = "Path")
	TObjectPtr<AENTArtificialIntelligencePath> NextPath;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Path")
	TArray<TObjectPtr<AENTArtificialIntelligencePath>> PreviousPaths;
#endif

	UPROPERTY(EditDefaultsOnly, Category = "Trace", meta = (Units = "cm"))
	float TraceLength = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Spline", meta = (Units = "cm"))
	float SplineHeight = 100.0f;

	UPROPERTY(EditInstanceOnly, Category = "Spline")
	bool bIsAClosedLoop = false;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundObjectTypes;

	UPROPERTY(EditInstanceOnly, Category = "Direction")
	TEnumAsByte<EAxis::Type> Direction = EAxis::Z;

	UPROPERTY(EditInstanceOnly, Category = "Direction")
	bool bInvertDirection = true;

	UPROPERTY(EditDefaultsOnly, Category = "Directon", meta = (ClampMin = 0.0f, Units = "cm"))
	float WallPointsOffset = 1.0f;

	void UpdatePoints(bool bInConstructionScript);

	void UpdateNavLink();

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "AI")
	TObjectPtr<APawn> AttachedAI;
#endif

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Direction")
	FVector GetDirection() const;

	FVector GetPointLocation(int32 PointIndex, float PawnHeight) const;

	FTransform GetFirstPointTransform(const ESplineCoordinateSpace::Type& CoordinateSpace) const;

	FTransform GetLastPointTransform(const ESplineCoordinateSpace::Type& CoordinateSpace) const;

	FTransform GetTransformAtAlpha(float Alpha) const;

	FTransform GetStartTransform() const {return GetTransformAtAlpha(0.0f);}

	bool IsAClosedLoop() const {return bIsAClosedLoop;}

	bool IsAtTheEndOfThePath(uint16 Index) const;

	USplineComponent* GetSpline() const {return Spline;}

	AENTArtificialIntelligencePath* GetNextPath() const {return NextPath;}

	/**
	 * @brief Should be used only onConstruction (use GetPointLocation() otherwise)
	 * @param PointIndex
	 * @param HitResult 
	 * @return 
	 */
	bool GetTracedPointLocation(int8 PointIndex, FHitResult& HitResult);

	bool IsOnFloor() const;

#if WITH_EDITORONLY_DATA
	bool AttachAI(APawn* AI);

	void DetachAI(APawn* AI);
#endif

#pragma region Debug

#if WITH_EDITORONLY_DATA

protected:
	UPROPERTY(EditInstanceOnly, Transient, Category = "Debug")
	bool bShowDebugMesh = false;

	UPROPERTY(EditInstanceOnly, Transient, Category = "Debug", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f, EditCondition = bShowDebugMesh))
	float DebugSplineAlpha = 0.0f;

	UPROPERTY(EditDefaultsOnly, Transient)
	TObjectPtr<USceneComponent> DebugMeshRootComp;

	UPROPERTY(EditDefaultsOnly, Transient)
	TObjectPtr<UStaticMeshComponent> DebugMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	TObjectPtr<UStaticMesh> DebugMesh;

	UPROPERTY(EditInstanceOnly, Category = "Debug", meta = (EditCondition = bShowDebugMesh))
	FRotator RotationOffset = FRotator(0.0f, -90.0f, 0.0f);

#endif

#pragma endregion
};
