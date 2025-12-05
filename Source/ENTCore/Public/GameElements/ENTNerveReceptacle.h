// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "ENTNerveReceptacle.generated.h"

class UENTInteractableComponent;
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
class UENTElectricityComponent;

UCLASS()
class ENTCORE_API AENTNerveReceptacle : public AActor
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNerveAnimationFinished);

public:
	AENTNerveReceptacle();

protected:
	UFUNCTION()

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnHoldStateChanged(bool bIsHolding);

#if WITH_EDITORONLY_DATA
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> NerveReceptacle;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UAkComponent> NerveReceptaclesNoises;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TObjectPtr<UAkAudioEvent> GrowlNoise;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TObjectPtr<UAkAudioEvent> EnabledNoise;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TObjectPtr<UAkAudioEvent> DisabledNoise;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Electricity")
	TObjectPtr<UENTElectricityComponent> ElectricityComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise", meta = (ClampMin = 0.0f, Units = cm))
	float NoiseRange = 2500.0f;

#if WITH_EDITORONLY_DATA
	/**
	 * @brief Set visibility to true if you want to edit it
	 */
	UPROPERTY(EditDefaultsOnly, Category = "NerveEnd")
	TObjectPtr<UStaticMeshComponent> NerveEndEditorMesh;
#endif

	UPROPERTY()
	FTransform NerveEndTargetTransform;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UENTInteractableComponent> InteractableComponent;

	UFUNCTION()
	void Interaction(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent);

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Nerve")
	TMap<AActor*, ENerveReactiveInteractionType> ObjectReactive;

private:
	UPROPERTY(EditAnywhere)
	FName ConnectedShaderTag;

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

#pragma region Electricity

	UFUNCTION()
	void OnElectricityAnimationStarted(AActor* LinkedActor);

	UFUNCTION()
	void OnElectricityRadiusFinished();

	UFUNCTION()
	void OnElectricityMovementUpdated(float Alpha);

	UFUNCTION()
	void OnElectricityMovementFinished();

	UFUNCTION()
	void OnElectricityOpacityFinished();

#pragma endregion 
};
