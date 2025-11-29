// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTCharacterMoveState.h"
#include "ENTCharacterFallState.generated.h"

class UAkRtpc;

UCLASS()
class ENTCORE_API UENTCharacterFallState : public UENTCharacterMoveState
{
	GENERATED_BODY()

public:
	UENTCharacterFallState();

protected:
	virtual void StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID) override;

	virtual void StateTick_Implementation(float DeltaTime) override;

	virtual void StateExit_Implementation(const EENTCharacterStateID& NextStateID) override;

#if WITH_EDITORONLY_DATA
	virtual void PostLoad() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

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

	/**
	 * @brief Will multiply the character velocity and then added to the noise range
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise", meta = (ClampMin = 0.0f, EditCondition = bDoesMakeNoise))
	float VelocityNoiseFactor = 1.0f;

	/**
	 * @brief EditDefaultsOnly is for debug purpose only
	 */
	UPROPERTY(EditDefaultsOnly, Transient, Category = "Noise", meta = (Units = "cm/s", ClampMin = 0.0f, EditCondition = bDoesMakeNoise))
	float CharacterVelocity = 200.0f;

#if WITH_EDITORONLY_DATA
	/**
	 * @brief Debug purpose only, Calculation is VelocityNoiseFactor * CharacterVelocity;
	 */
	UPROPERTY(VisibleDefaultsOnly, Transient, Category = "Noise|Debug", meta = (Units = cm))
	float DebugNoiseRange = 0.0f;
#endif

	virtual void EmitNoise() override;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkRtpc> SpeedVolume;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkAudioEvent> StartWindEvent;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkAudioEvent> StopWindEvent;

	/**
	 * @brief Inclusive value
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Noise", meta = (Units = "cm/s"))
	float WindRequiredVelocity = 2000.0f;

	bool bIsWindSFXPlaying = false;

	void ManageWindSFX();

private:
	bool bCanDoCoyoteTime = false;

	float CoyoteTime = 0.0f;

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
