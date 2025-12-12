// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTMeshInstances.h"

#include "Components/InstancedStaticMeshComponent.h"

AENTMeshInstances::AENTMeshInstances()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("MeshInstances"));
	SetRootComponent(MeshInstances);
	MeshInstances->SetMobility(EComponentMobility::Static);
}

void AENTMeshInstances::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	MeshInstances->ClearInstances();
	MeshInstances->SetStaticMesh(StaticMesh);
	MeshInstances->AddInstances(InstancesTransforms, false, false, true);

#if WITH_EDITORONLY_DATA
	InstanceNumber = MeshInstances->GetNumInstances();
#endif
}

#if WITH_EDITOR
void AENTMeshInstances::BuildInstances(UStaticMesh* TargetMesh, const TArray<FTransform>& LocalTransforms)
{
	StaticMesh = TargetMesh;
	InstancesTransforms = LocalTransforms;
	RerunConstructionScripts();
}
#endif

