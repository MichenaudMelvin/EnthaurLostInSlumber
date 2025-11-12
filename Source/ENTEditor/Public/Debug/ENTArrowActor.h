// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ENTArrowActor.generated.h"

class UArrowComponent;

UCLASS()
class ENTEDITOR_API AENTArrowActor : public AActor
{
	GENERATED_BODY()

public:
	AENTArrowActor();

protected:
	UPROPERTY()
	TObjectPtr<UArrowComponent> ArrowComponent;

public:
	void SetArrowDimensions(float Size, float Length) const;

	void SetArrowColor(const FColor& Color) const;

	void SetArrowDirection(const FVector& Direction);

	TObjectPtr<UArrowComponent> GetArrow() const {return ArrowComponent;}
};
