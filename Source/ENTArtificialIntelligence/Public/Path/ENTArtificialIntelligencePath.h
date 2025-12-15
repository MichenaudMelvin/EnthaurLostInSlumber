// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ENTArtificialIntelligencePath.generated.h"

class UBoxComponent;

namespace ESplineCoordinateSpace
{
	enum Type : int;
}

class UNavLinkCustomComponent;
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

	UPROPERTY(EditInstanceOnly, Category = "Spline")
	TEnumAsByte<EAxis::Type> Direction = EAxis::Z;

	UPROPERTY(EditInstanceOnly, Category = "Spline")
	bool bInvertDirection = true;

	UPROPERTY(EditAnywhere, Category = "Spline", meta = (ClampMin = 0.0f, Units = "cm"))
	float WallPointsOffset = 1.0f;

	void UpdatePoints(bool bInConstructionScript);

	void UpdateNavLink();

	/**
	 * @brief Blackboard key for AI
	 */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName AIPathKeyName = "AIPath";

	/**
	 * @brief Blackboard key for AI
	 */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName PathIndexKeyName = "PathIndex";

	/**
	 * @brief Blackboard key for AI
	 */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName PathDirectionKeyName = "PathDirection";

	/**
	 * @brief Blackboard key for AI
	 */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName JumpKeyName = "RequestJump";

	/**
	 * @brief Blackboard key for AI
	 */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName JumpLocationKeyName = "JumpLocation";

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

	FTransform GetTransformAtAlpha(float Alpha, int8 SplineDirection) const;

	FTransform GetStartTransform(int8 SplineDirection) const {return GetTransformAtAlpha(0.0f, SplineDirection);}

	FTransform GetEndTransform(int8 SplineDirection) const {return GetTransformAtAlpha(1.0f, SplineDirection);}

	FVector GetNavLinkLocation(int32 PathDirection) const;

	float GetWallOffset() const {return WallPointsOffset;}

	bool IsAClosedLoop() const {return bIsAClosedLoop;}

	bool IsAtTheEndOfThePath(uint16 Index, int32 PathDirection) const;

	bool IsAtTheEndOfThePath(const FVector& ActorLocation, float ActorHeight, int32 PathDirection, float Tolerance) const;

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

#pragma region NavLinks

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NavLink")
	TObjectPtr<UNavLinkCustomComponent> FirstNavLink;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NavLink")
	TObjectPtr<UNavLinkCustomComponent> SecondNavLink;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "NavLink", meta = (MakeEditWidget))
	FVector FirstNavLinkLocation = FVector::ZeroVector;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "NavLink", meta = (MakeEditWidget))
	FVector SecondNavLinkLocation = FVector(500.0f, 0.0f, 0.0f);

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Transient, Category = "NavLink")
	bool bShowPlatform = false;

	UPROPERTY(EditInstanceOnly, Category = "NavLink")
	bool bUsePlatform = true;

	UPROPERTY(EditInstanceOnly, Transient, Category = "NavLink")
	bool bIgnoreGroundTrace = false;

	UPROPERTY(EditAnywhere, Category = "NavLink", meta = (Units = cm, ClampMin = 0.0f))
	float GroundTraceLength = 500.0f;

	UPROPERTY(Transient)
	TObjectPtr<UArrowComponent> FistNavLinkDebugArrow;

	UPROPERTY(Transient)
	TObjectPtr<UArrowComponent> SecondNavLinkDebugArrow;

	UPROPERTY(Transient)
	TObjectPtr<UArrowComponent> FistNavLinkPlatformDebugArrow;

	UPROPERTY(Transient)
	TObjectPtr<UArrowComponent> SecondNavLinkPlatformDebugArrow;

	/**
	 * @brief This component is just used to build a navmesh and create a link between 2 nav meshes, this is an EditorOnly component, (in editor the component is destroyed at the beginPlay)
	 */
	UPROPERTY(EditDefaultsOnly, Category = "NavLink")
	TObjectPtr<UStaticMeshComponent> NavLinkPlatform;

	UPROPERTY(EditDefaultsOnly, Category = "NavLink")
	TObjectPtr<UBoxComponent> NavModifier;

	UPROPERTY(EditInstanceOnly, Category = "NavLink")
	FVector PlatformOffset = FVector::ZeroVector;

	/**
	 * @brief PlatformScale * MeshSize.Y is the result you will see in the engine
	 */
	UPROPERTY(EditAnywhere, Category = "NavLink", meta = (ClampMin = 0.0f))
	float PlatformScale = 10.0f;

	UPROPERTY(EditAnywhere, Category = "NavLink", meta = (Units = cm, ClampMin = 0.0f))
	float NavLinkOffset = 500.0f;

	UPROPERTY(EditInstanceOnly, Category = "NavLink")
	bool bPointPlatform = false;

	UPROPERTY(EditInstanceOnly, Category = "NavLink", DisplayName = "OverridenScale", meta = (EditCondition = bPointPlatform))
	FVector2D OverridenNavLinkScale = FVector2D(10.0f);

	UPROPERTY(EditInstanceOnly, Category = "NavLink", meta = (EditCondition = bPointPlatform))
	TEnumAsByte<EAxis::Type> NavLinksPointAxis = EAxis::X;

	void BuildNavLinkPlatform() const;
#endif

	void NotifyLinkReached(UNavLinkCustomComponent* NavLinkCustomComponent, UObject* PathingAgent, const UE::Math::TVector<double>& Destination);

#pragma endregion

#pragma region Debug

#if WITH_EDITORONLY_DATA

protected:
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bShowTraces = false;

	UPROPERTY(EditInstanceOnly, Transient, Category = "Debug")
	bool bShowDebugMesh = false;

	UPROPERTY(EditInstanceOnly, Transient, Category = "Debug", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f, EditCondition = bShowDebugMesh))
	float DebugSplineAlpha = 0.0f;

	UPROPERTY(EditInstanceOnly, Transient, Category = "Debug", meta = (EditCondition = bShowDebugMesh))
	bool DebugInvertMeshDirection = false;

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
