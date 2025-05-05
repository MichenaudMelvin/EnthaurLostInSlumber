// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIPath.generated.h"

class USplineComponent;

UCLASS()
class DEPTHAIMODULE_API AAIPath : public AActor
{
	GENERATED_BODY()

public:
	AAIPath();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USplineComponent> Spline;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	TObjectPtr<UBillboardComponent> BillboardComponent;

	UPROPERTY(Transient)
	TArray<TObjectPtr<class UArrowComponent>> Arrows;
#endif

	UPROPERTY(EditDefaultsOnly, Category = "Trace", meta = (Units = "cm"))
	float TraceLength = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Spline", meta = (Units = "cm"))
	float SplineHeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundObjectTypes;

	UPROPERTY(EditInstanceOnly, Category = "Direction", meta = (EditCondition = "!bAutoDirection"))
	TEnumAsByte<EAxis::Type> Direction = EAxis::Z;

	UPROPERTY(EditInstanceOnly, Category = "Direction", meta = (EditCondition = "!bAutoDirection"))
	bool bInvertDirection = true;

	void UpdatePoints(bool bInConstructionScript);

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "AI")
	TObjectPtr<APawn> AttachedAI;
#endif

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Direction")
	FVector GetDirection() const;

	FVector GetPointLocation(int8 PointIndex, float PawnHeight) const;

	USplineComponent* GetSpline() const {return Spline;}

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
};
