// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GravityPawnMovement.generated.h"

/**
 * @brief Use the owner to tick the component
 */
UCLASS(ClassGroup = ("AI"), meta = (BlueprintSpawnableComponent))
class DEPTHAIMODULE_API UGravityPawnMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()

public:
	UGravityPawnMovement();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gravity", meta = (ClampMin = 0.0f))
	float GravityScale = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gravity")
	FVector GravityDirection = FVector::DownVector;

public:
	void SetGravityScale(const float InGravityScale) {GravityScale = InGravityScale;}

	void SetGravityDirection(const FVector& InGravityDirection) {GravityDirection = InGravityDirection;}

	UFUNCTION(BlueprintCallable, Category = "Gravity")
	void ApplyGravity(float DeltaTime);
};
