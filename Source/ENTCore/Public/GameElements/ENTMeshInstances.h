// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ENTMeshInstances.generated.h"

UCLASS()
class ENTCORE_API AENTMeshInstances : public AActor
{
	GENERATED_BODY()

public:
	AENTMeshInstances();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	UFUNCTION(BlueprintCallable, Category = "Instances", meta = (DevelopmentOnly))
	void BuildInstances(UStaticMesh* TargetMesh, const TArray<FTransform>& LocalTransforms);
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Instances")
	TObjectPtr<UInstancedStaticMeshComponent> MeshInstances;

	UPROPERTY(EditInstanceOnly, Category = "Instances")
	TObjectPtr<UStaticMesh> StaticMesh;

	UPROPERTY(EditInstanceOnly, Category = "Instances", meta = (MakeEditWidget))
	TArray<FTransform> InstancesTransforms;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Instances")
	int32 InstanceNumber = 0;
#endif
};
