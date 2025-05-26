// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NerveReceptacle.generated.h"

class UAkAudioEvent;
enum class ENerveReactiveInteractionType : uint8;
class INerveReactive;
class USphereComponent;

UCLASS()
class PROTOPROFONDEURS_API ANerveReceptacle : public AActor
{
	GENERATED_BODY()

public:
	ANerveReceptacle();
	void TriggerLinkedObjects(class ANerve* Nerve);

	// Create Event
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
	void OnNerveConnect();

protected:
	void PlayElectricityAnimation(ANerve* Nerve);

	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<AActor*, ENerveReactiveInteractionType> ObjectReactive;

	UPROPERTY(EditDefaultsOnly, Category = "Noise")
	TObjectPtr<UAkAudioEvent> ReceptacleEnabledNoise;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> NerveReceptacle;

	UPROPERTY(EditAnywhere)
	FName ConnectedShaderTag;

	UPROPERTY()
	class AElectricityFeedback* NerveElectricityFeedback;

	UPROPERTY()
	ANerve* KeepInMemoryNerve;
};
