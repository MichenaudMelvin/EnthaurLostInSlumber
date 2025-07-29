// Fill out your copyright notice in the Description page of Project Settings.


#include "ENTGravityPawnMovement.h"


UENTGravityPawnMovement::UENTGravityPawnMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UENTGravityPawnMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ApplyGravity(DeltaTime);
}

bool UENTGravityPawnMovement::IsMovingOnGround() const
{
	return VerticalVelocity == 0;
}

bool UENTGravityPawnMovement::IsFalling() const
{
	return VerticalVelocity != 0;
}

void UENTGravityPawnMovement::ApplyGravity(float DeltaTime)
{
	// gravity = m/s²

	// m/s
	VerticalVelocity = GetWorld()->GetDefaultGravityZ() * DeltaTime * GravityScale;

	// m
	FVector Delta = VerticalVelocity * DeltaTime * (GravityDirection * -1);

	FHitResult HitResult;
	bool bHit = MoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, &HitResult);

	if (bHit)
	{
		VerticalVelocity = 0.0f;
	}
}
