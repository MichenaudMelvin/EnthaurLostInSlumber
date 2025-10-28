// Fill out your copyright notice in the Description page of Project Settings.


#include "ENTGravityPawnMovement.h"


UENTGravityPawnMovement::UENTGravityPawnMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UENTGravityPawnMovement::BeginPlay()
{
	Super::BeginPlay();

	DefaultGravityScale = GravityScale;
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

#if WITH_EDITOR
void UENTGravityPawnMovement::PostLoad()
{
	Super::PostLoad();

	DefaultGravityScale = GravityScale;
}

void UENTGravityPawnMovement::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.GetPropertyName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UENTGravityPawnMovement, GravityScale))
	{
		DefaultGravityScale = GravityScale;
	}
}
#endif

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
