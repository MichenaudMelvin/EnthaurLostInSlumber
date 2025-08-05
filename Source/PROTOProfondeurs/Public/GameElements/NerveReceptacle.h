// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NerveReceptacle.generated.h"

class UAkComponent;

UENUM(BlueprintType)
enum class ENerveReactiveInteractionType : uint8
{
	Activate,
	ForceDefaultState
};

class ANerve;
class UAkAudioEvent;
class INerveReactive;
class USphereComponent;
class AElectricityFeedback;

UCLASS()
class PROTOPROFONDEURS_API ANerveReceptacle : public AActor
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNerveAnimationFinished);

public:
	ANerveReceptacle();

protected:
	virtual void BeginPlay() override;

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

	void PlayElectricityAnimation(ANerve* Nerve);

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Nerve")
	TMap<AActor*, ENerveReactiveInteractionType> ObjectReactive;

private:
	UPROPERTY(EditAnywhere)
	FName ConnectedShaderTag;

	UPROPERTY()
	TObjectPtr<AElectricityFeedback> NerveElectricityFeedback;

	UPROPERTY()
	TObjectPtr<ANerve> KeepInMemoryNerve;

	bool IsConnected;

public:
	void TriggerLinkedObjects(class ANerve* Nerve);

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
