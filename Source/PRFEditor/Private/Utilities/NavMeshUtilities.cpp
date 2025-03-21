// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/NavMeshUtilities.h"
#include "VRScoutingInteractor.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"

UNavMeshUtilities::UNavMeshUtilities()
{
	
}

void UNavMeshUtilities::AutoScale()
{
	TArray<AActor*> SelectedActors = UVRScoutingInteractor::GetSelectedActors();

	ANavMeshBoundsVolume* NavMesh = FindNavMesh(SelectedActors);

	if (!NavMesh)
	{
		return;
	}

	TArray<AActor*> ActorsToCheck;

	if (SelectedActors.Num() <= 1)
	{
		UGameplayStatics::GetAllActorsOfClass(NavMesh, AActor::StaticClass(), ActorsToCheck);
	}
	else
	{
		ActorsToCheck = SelectedActors;
	}

	FVector Center;
	FVector Extent;
	UGameplayStatics::GetActorArrayBounds(ActorsToCheck, true, Center, Extent);

	NavMesh->SetActorLocation(Center);
	NavMesh->SetActorScale3D(Extent / 100.0f);
	// build navigation
}

ANavMeshBoundsVolume* UNavMeshUtilities::FindNavMesh(const TArray<AActor*>& Actors) const
{
	for (AActor* Actor : Actors)
	{
		ANavMeshBoundsVolume* NavMesh = Cast<ANavMeshBoundsVolume>(Actor);
		if (NavMesh)
		{
			return NavMesh;
		}
	}

	return nullptr;
}
