// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTPhysicConstraint.h"
#include "ENTNervePhysicConstraint.generated.h"

enum class EENTCharacterStateID:uint8;
class UENTCharacterState;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ENTCORE_API UENTNervePhysicConstraint : public UENTPhysicConstraint
{
	GENERATED_BODY()

private:
	UPROPERTY()
	float CameraBobbingTime = 0.0f;

	UPROPERTY()
	float BaseFrequency = 0.7f;

	UPROPERTY()
	float BaseAmplitude = 2.f;

protected:
	virtual void UpdateDefaultMaxSpeed(UENTCharacterState* State, const EENTCharacterStateID& StateID) override;
	
public:
	virtual void Init(AENTNerve* vLinkedNerve, ACharacter* vPlayerCharacter) override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void ReleasePlayer(const bool DetachFromPlayer = false) override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
