// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTWeakZoneInterface.h"
#include "GameFramework/Actor.h"
#include "ENTTeleportationVein.generated.h"

UCLASS()
class ENTCORE_API AENTTeleportationVein : public AActor, public IENTWeakZoneInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AENTTeleportationVein();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> MapToLoad;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVeinInteracted);
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnVeinInteracted OnVeinInteracted;
	
};
