// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ENTJumpSpline.generated.h"

class USplineComponent;

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API AENTJumpSpline : public AActor
{
	GENERATED_BODY()

public:
	AENTJumpSpline();

protected:
	UPROPERTY()
	TObjectPtr<USplineComponent> JumpSpline;

public:
	void InitSpline(const FTransform& StartTransform, const FTransform& EndTransform) const;

	FVector GetLocationAtAlpha(float Alpha) const;

#if WITH_EDITORONLY_DATA
	void ShowSpline(const FLinearColor& SplineColor = FLinearColor::White) const;
#endif
};
