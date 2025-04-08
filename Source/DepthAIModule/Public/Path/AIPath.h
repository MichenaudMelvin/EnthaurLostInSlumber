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
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USplineComponent> Spline;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UBillboardComponent> BillboardComponent;

	UPROPERTY()
	TArray<TObjectPtr<class UArrowComponent>> Arrows;
#endif

	UPROPERTY(EditDefaultsOnly, Category = "Trace", meta = (Units = "cm"))
	float TraceLength = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Spline", meta = (Units = "cm"))
	float SplineHeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundObjectTypes;

	UPROPERTY()
	bool bAutoDirection = false;

	UPROPERTY(EditInstanceOnly, Category = "Direction", meta = (EditCondition = "!bAutoDirection"))
	TEnumAsByte<EAxis::Type> Direction = EAxis::Z;

	UPROPERTY(EditInstanceOnly, Category = "Direction", meta = (EditCondition = "!bAutoDirection"))
	bool bInvertDirection = true;

	void UpdatePoints(bool bInConstructionScript);

	FVector GetDirection();

public:
	FVector GetPointLocation(int8 PointIndex) const;

	USplineComponent* GetSpline() const {return Spline;}
};
