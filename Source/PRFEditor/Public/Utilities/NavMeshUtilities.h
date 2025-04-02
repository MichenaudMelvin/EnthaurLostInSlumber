// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Classes/ActorActionUtility.h"
#include "NavMeshUtilities.generated.h"

class ANavMeshBoundsVolume;

UCLASS()
class PRFEDITOR_API UNavMeshUtilities : public UActorActionUtility
{
	GENERATED_BODY()

public:
	UNavMeshUtilities();

protected:
	UFUNCTION(BlueprintCallable, Category = "NavMesh", CallInEditor)
	void AutoScale();

	ANavMeshBoundsVolume* FindNavMesh(const TArray<AActor*>& Actors) const;
};
