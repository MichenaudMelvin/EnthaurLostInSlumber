// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GravityPawnMovement.h"


UGravityPawnMovement::UGravityPawnMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGravityPawnMovement::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ApplyGravity(DeltaTime);
}

void UGravityPawnMovement::ApplyGravity(float DeltaTime)
{
	// gravity = m/s²

	// m/s
	VerticalVelocity = GetWorld()->GetDefaultGravityZ() * DeltaTime * GravityScale;

	// m
	FVector Delta = VerticalVelocity * DeltaTime * (GravityDirection * -1);

	bool bHit = MoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true);

	if (bHit)
	{
		VerticalVelocity = 0.0f;
	}
}
