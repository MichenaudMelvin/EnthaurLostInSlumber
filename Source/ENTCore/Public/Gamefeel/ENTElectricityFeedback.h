// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AkGameplayTypes.h"
#include "GameFramework/Actor.h"
#include "ENTElectricityFeedback.generated.h"

class UAkComponent;

UCLASS()
class ENTCORE_API AENTElectricityFeedback : public AActor
{
	GENERATED_BODY()

public:
	AENTElectricityFeedback();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UPostProcessComponent> Electricity;

	UPROPERTY(EditDefaultsOnly, Category = "Noises")
	TObjectPtr<UAkComponent> ElectricityNoises;

	UPROPERTY(BlueprintReadWrite, Category = "Electricity")
	float Radius = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Electricity")
	TObjectPtr<UMaterialInstanceDynamic> Material;

	UFUNCTION()
	void PlayNoise(EAkCallbackType CallbackType, UAkCallbackInfo* CallbackInfo);

public:
	float GetRadius() const {return Radius;}

	void SetRadius(float InRadius) {Radius = InRadius;}

	TObjectPtr<UMaterialInstanceDynamic> GetMaterial() const {return Material;}
};
