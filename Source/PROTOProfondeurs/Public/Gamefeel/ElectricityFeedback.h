// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AkGameplayTypes.h"
#include "GameFramework/Actor.h"
#include "ElectricityFeedback.generated.h"

class UAkComponent;

UCLASS()
class PROTOPROFONDEURS_API AElectricityFeedback : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AElectricityFeedback();

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UPostProcessComponent> Electricity;

	UPROPERTY(EditDefaultsOnly, Category = "Noises")
	TObjectPtr<UAkAudioEvent> DestroyedEvent;

	UPROPERTY(EditDefaultsOnly, Category = "Noises")
	TObjectPtr<UAkComponent> ElectricityNoises;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> Material;

	UFUNCTION()
	void PlayNoise(EAkCallbackType CallbackType, UAkCallbackInfo* CallbackInfo);

public:
	float GetRadius() const {return Radius;}

	void SetRadius(float InRadius) {Radius = InRadius;}

	TObjectPtr<UMaterialInstanceDynamic> GetMaterial() const {return Material;}
};
