// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "ENTElectricityComponent.generated.h"


class AENTElectricityFeedback;

UCLASS(ClassGroup=(Electricity), meta=(BlueprintSpawnableComponent))
class ENTCOMPONENTS_API UENTElectricityComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnElectricityAnimationStarted, AActor*, LinkedActor);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnElectricityRadiusFinished);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnElectricityMovementUpdated, float, Alpha);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnElectricityMovementFinished);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnElectricityOpacityFinished);

public:
	UENTElectricityComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PlayElectricityAnimation(AActor* LinkedActor = nullptr);

	UPROPERTY(BlueprintAssignable, Category="Electricity|Events")
	FOnElectricityAnimationStarted OnElectricityAnimationStarted;

	UPROPERTY(BlueprintAssignable, Category="Electricity|Events")
	FOnElectricityRadiusFinished OnElectricityRadiusFinished;

	UPROPERTY(BlueprintAssignable, Category="Electricity|Events")
	FOnElectricityMovementUpdated OnElectricityMovementUpdated;

	UPROPERTY(BlueprintAssignable, Category="Electricity|Events")
	FOnElectricityMovementFinished OnElectricityMovementFinished;

	UPROPERTY(BlueprintAssignable, Category="Electricity|Events")
	FOnElectricityOpacityFinished OnElectricityOpacityFinished;

	TObjectPtr<AENTElectricityFeedback> GetElectricityFeedback() const {return ElectricityFeedback;}

	bool IsAnimRunning() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Electricity")
	TSubclassOf<AActor> ElectricityFeedbackClass;

	UPROPERTY(EditDefaultsOnly, Category = "Electricity", meta = (ClampMin = 1.0f, Units = "cm/s"))
	float ElectricitySpeed = 750.0f;

	UPROPERTY()
	AActor* ElectricityFeedbackInstance;

private:
	UPROPERTY()
	TObjectPtr<AENTElectricityFeedback> ElectricityFeedback;

#pragma region ElectricityRadius

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Electricity|Radius")
	TObjectPtr<UCurveFloat> FirstElectricityRadiusCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Electricity|Radius")
	TObjectPtr<UCurveFloat> SecondElectricityRadiusCurve;

	FTimeline ElectricityRadiusTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Electricity|Radius", meta = (Units = s))
	float FirstElectricityRadiusDuration = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Electricity|Radius", meta = (Units = s))
	float SecondElectricityRadiusDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Electricity|Radius")
	float FirstRadiusTarget = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Electricity|Radius")
	float SecondRadiusTarget = 200.0f;

	float StartRadiusTarget = 0.0f;

	float EndRadiusTarget = 0.0f;

	UFUNCTION()
	void ElectricityRadiusUpdate(float Alpha);

	UFUNCTION()
	void ElectricityRadiusFinished();

#pragma endregion

#pragma region ElectricityMovement

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Electricity|Movement")
	TObjectPtr<UCurveFloat> ElectricityMovementCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Electricity|Movement", meta = (Units = s))
	float ElectricityMovementDuration = 1.5f;

	FTimeline ElectricityMovementTimeline;

	UFUNCTION()
	void ElectricityMovementUpdate(float Alpha);

	UFUNCTION()
	void ElectricityMovementFinished();

#pragma endregion

#pragma region ElectricityOpacity

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Electricity|Opacity")
	TObjectPtr<UCurveFloat> ElectricityOpacityCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Electricity|Opacity")
	FName ElectricityOpacityParam = "Opacity";

	UPROPERTY(EditDefaultsOnly, Category = "Electricity|Opacity", meta = (Units = s))
	float ElectricityOpacityDuration = 2.0f;

	FTimeline ElectricityOpacityTimeline;

	UFUNCTION()
	void ElectricityOpacityUpdate(float Alpha);

	UFUNCTION()
	void ElectricityOpacityFinished();

#pragma endregion

};
