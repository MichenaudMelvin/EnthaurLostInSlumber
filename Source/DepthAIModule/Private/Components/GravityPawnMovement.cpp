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
	Velocity = ((GetWorld()->GetDefaultGravityZ() * -1.0f) * GravityDirection * GravityScale * DeltaTime);
}
