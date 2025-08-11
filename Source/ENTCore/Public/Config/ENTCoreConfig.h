// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AkSwitchValue.h"
#include "Engine/DeveloperSettings.h"
#include "Player/Camera/ENTViewBobbing.h"
#include "ENTCoreConfig.generated.h"

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Core Config"))
class ENTCORE_API UENTCoreConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UENTCoreConfig();

#if WITH_EDITOR
protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	UPROPERTY(Config, EditDefaultsOnly, Category = "Character|Camera")
	TSubclassOf<UENTViewBobbing> ViewBobbingClass = UENTViewBobbing::StaticClass();

	UPROPERTY(Config, EditDefaultsOnly, Category = "Character|Inputs", meta = (UIMin = 0.0f, UIMax = 1.0f, ClampMin = 0.0f, ClampMax = 1.0f))
	float MoveInputThreshold = 0.1f;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Character|Grab")
	FVector PawnGrabObjectOffset = FVector::ZeroVector;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Physics")
	FFloatRange CameraPropulsion = FFloatRange(0.0f, 0.7f);

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Physics|Traces")
	TEnumAsByte<ECollisionChannel> InteractionTraceChannel = ECC_EngineTraceChannel1;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Physics|Traces")
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundObjectTypes;

protected:
	UPROPERTY(Config, EditDefaultsOnly, Category = "Physics|Surface")
	TMap<TEnumAsByte<EPhysicalSurface>, TSoftObjectPtr<UAkSwitchValue>> SurfaceNoises;

public:
	UFUNCTION(BlueprintCallable, Category = "Physics|Surface")
	UAkSwitchValue* FindNoise(const EPhysicalSurface& Surface) const;
};
