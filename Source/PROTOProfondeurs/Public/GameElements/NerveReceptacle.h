// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NerveReceptacle.generated.h"

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

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> NerveReceptacle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Components")
	TObjectPtr<USphereComponent> Collision;

	void PlayElectricityAnimation(ANerve* Nerve);

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Nerve")
	TMap<AActor*, ENerveReactiveInteractionType> ObjectReactive;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkAudioEvent> ReceptacleEnabledNoise;

private:
	UPROPERTY(EditAnywhere)
	FName ConnectedShaderTag;

	UPROPERTY()
	TObjectPtr<AElectricityFeedback> NerveElectricityFeedback;

	UPROPERTY()
	TObjectPtr<ANerve> KeepInMemoryNerve;

public:
	void TriggerLinkedObjects(class ANerve* Nerve);

	// Create Event
	UFUNCTION(BlueprintImplementableEvent, Category = "Nerve")
	void OnNerveConnect();

	UPROPERTY(BlueprintAssignable, Category = "Nerve")
	FOnNerveAnimationFinished OnNerveAnimationFinished;
};
