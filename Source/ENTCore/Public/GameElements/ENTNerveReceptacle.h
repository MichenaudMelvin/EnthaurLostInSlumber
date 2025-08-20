// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "ENTNerveReceptacle.generated.h"

class UAkComponent;

UENUM(BlueprintType)
enum class ENerveReactiveInteractionType : uint8
{
	Activate,
	ForceDefaultState
};

class AENTNerve;
class UAkAudioEvent;
class IENTActivation;
class USphereComponent;
class AENTElectricityFeedback;

UCLASS()
class ENTCORE_API AENTNerveReceptacle : public AActor
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNerveAnimationFinished);

public:
	AENTNerveReceptacle();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

#if WITH_EDITORONLY_DATA
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> NerveReceptacle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Components")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UAkComponent> NerveReceptaclesNoises;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TObjectPtr<UAkAudioEvent> GrowlNoise;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TObjectPtr<UAkAudioEvent> EnabledNoise;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TObjectPtr<UAkAudioEvent> DisabledNoise;

	UPROPERTY(EditDefaultsOnly, Category = "Electricity")
	TSubclassOf<AActor> ElectricityFeedbackClass;

#if WITH_EDITORONLY_DATA
	/**
	 * @brief Set visibility to true if you want to edit it
	 */
	UPROPERTY(EditDefaultsOnly, Category = "NerveEnd")
	TObjectPtr<UStaticMeshComponent> NerveEndEditorMesh;
#endif

	UPROPERTY()
	FTransform NerveEndTargetTransform;

	UPROPERTY(EditDefaultsOnly, Category = "Electricity", meta = (ClampMin = 1.0f, Units = "cm/s"))
	float ElectricitySpeed = 750.0f;

	void PlayElectricityAnimation(AENTNerve* Nerve);

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Nerve")
	TMap<AActor*, ENerveReactiveInteractionType> ObjectReactive;

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

private:
	UPROPERTY(EditAnywhere)
	FName ConnectedShaderTag;

	UPROPERTY()
	TObjectPtr<AENTElectricityFeedback> NerveElectricityFeedback;

	UPROPERTY()
	TObjectPtr<AENTNerve> LinkedNerve;

	bool IsConnected;

public:
	void TriggerLinkedObjects(class AENTNerve* Nerve);

	// Create Event
	UFUNCTION(BlueprintImplementableEvent, Category = "Nerve")
	void OnNerveConnect();

	UFUNCTION(BlueprintImplementableEvent, Category = "Nerve")
	void OnNerveDisconnect(bool isConnected);

	const FTransform& GetAttachTransform() const {return NerveEndTargetTransform;}

	UPROPERTY(BlueprintAssignable, Category = "Nerve")
	FOnNerveAnimationFinished OnNerveAnimationFinished;

	bool CanTheNerveBeTaken() const;

	void DisableReceptacle();
};
