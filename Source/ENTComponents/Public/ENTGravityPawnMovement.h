// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "ENTGravityPawnMovement.generated.h"

UCLASS(ClassGroup = "Pawn", meta = (BlueprintSpawnableComponent))
class ENTCOMPONENTS_API UENTGravityPawnMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()

public:
	UENTGravityPawnMovement();

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool IsMovingOnGround() const override;

	virtual bool IsFalling() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gravity", meta = (ClampMin = 0.0f))
	float GravityScale = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gravity")
	FVector GravityDirection = FVector::DownVector;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Gravity")
	float VerticalVelocity = 0.0f;

public:
	void SetGravityScale(const float InGravityScale) {GravityScale = InGravityScale;}

	void SetGravityDirection(const FVector& InGravityDirection) {GravityDirection = InGravityDirection;}

	UFUNCTION(BlueprintCallable, Category = "Gravity")
	void ApplyGravity(float DeltaTime);
};
