// Fill out your copyright notice in the Description page of Project Settings.


#include "Debug/ENTArrowActor.h"

#include "Components/ArrowComponent.h"

AENTArrowActor::AENTArrowActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	SetRootComponent(ArrowComponent);

	ArrowComponent->SetHiddenInGame(false);
	ArrowComponent->SetVisibility(true);

	bIsEditorOnlyActor = true;
}

void AENTArrowActor::SetArrowDimensions(float Size, float Length) const
{
	ArrowComponent->SetArrowSize(Size);
	ArrowComponent->SetArrowLength(Length);
}

void AENTArrowActor::SetArrowColor(const FColor& Color) const
{
	ArrowComponent->SetArrowFColor(Color);
}

void AENTArrowActor::SetArrowDirection(const FVector& Direction)
{
	SetActorRotation(FRotationMatrix::MakeFromX(Direction).Rotator());
}

