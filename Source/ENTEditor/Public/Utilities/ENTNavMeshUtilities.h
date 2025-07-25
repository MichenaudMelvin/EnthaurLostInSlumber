// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Classes/ActorActionUtility.h"
#include "ENTNavMeshUtilities.generated.h"

class ANavMeshBoundsVolume;

UCLASS()
class ENTEDITOR_API UENTNavMeshUtilities : public UActorActionUtility
{
	GENERATED_BODY()

public:
	UENTNavMeshUtilities();

protected:
	UFUNCTION(BlueprintCallable, Category = "NavMesh", CallInEditor)
	void AutoScale();

	ANavMeshBoundsVolume* FindNavMesh(const TArray<AActor*>& Actors) const;
};
