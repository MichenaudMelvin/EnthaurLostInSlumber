// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTCharacterMoveState.h"
#include "ENTCharacterFallState.generated.h"

UCLASS()
class ENTCORE_API UENTCharacterFallState : public UENTCharacterMoveState
{
	GENERATED_BODY()

public:
	UENTCharacterFallState();

protected:
	virtual void StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Fall", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float AirControl = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Fall", meta = (ClampMin = 0.0f))
	float GravityScale = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Fall", DisplayName = "CoyoteTime", meta = (ClampMin = 0.0f, Units = "s"))
	float CoyoteTimeMaxDuration = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Fall", meta = (ClampMin = 0.0f))
	float FallingDeceleration = 100.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Movement|Projection")
	FVector ProjectionVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Movement|Projection")
	bool bOverrideCurrentVelocity = false;

	UPROPERTY(EditDefaultsOnly, Category = "SpikeBrake", meta = (ClampMin = 0.0f, Units = "s"))
	float SpikeBrakeMaxPressedDuration = 0.75f;

	UPROPERTY(EditDefaultsOnly, Category = "SpikeBrake", meta = (ClampMin = 0.0f, Units = "cm"))
	float SpikeBrakeTraceLength = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "SpikeBrake", meta = (ClampMin = 0.0f, Units = "s"))
	float SpikeBrakeDelay = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "SpikeBrake")
	TEnumAsByte<ETraceTypeQuery> SpikeBrakeTraceTypeQuery;

private:
	bool bCanDoCoyoteTime = false;

	float CoyoteTime = 0.0f;

	float SpikeBrakePressedDuration = 0.0f;

	float SpikeBrakeTimer = 0.0f;

	bool bHasPressedInteraction = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Movement|Projection")
	void SetProjectionVelocity(const FVector& Velocity, bool bOverrideVelocity);

	UFUNCTION(BlueprintCallable, Category = "Movement|Projection")
	void LaunchCharacter() const;

#if WITH_EDITORONLY_DATA
	/**
	 * @brief Because reflection isn't working
	 * @return 
	 */
	float GetGravityScale() const {return GravityScale;}
#endif
};
