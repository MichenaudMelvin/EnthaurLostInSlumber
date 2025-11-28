// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ENTArtificialIntelligenceSubsystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/ENTArtificialIntelligenceInterface.h"

#pragma region Defaults

void UENTArtificialIntelligenceSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UENTArtificialIntelligenceSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	NoiseActor = InWorld.SpawnActor(AActor::StaticClass());
	NoiseActor->AddComponentByClass(USceneComponent::StaticClass(), false, FTransform::Identity, false);

#if WITH_EDITOR
	NoiseActor->SetActorLabel("NoiseActor", true);
#endif
}

void UENTArtificialIntelligenceSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (TObjectPtr<UObject> AI : AIList)
	{
		if(AI)
		{
			IENTArtificialIntelligenceInterface::Execute_TickAI(AI, DeltaTime);
		}
	}
}

#pragma endregion

#pragma region AI

void UENTArtificialIntelligenceSubsystem::AddAI(UObject* AIObject)
{
	if (!AIObject)
	{
		return;
	}

	if(AIObject->Implements<UENTArtificialIntelligenceInterface>())
	{
		AIList.Add(AIObject);
	}
}

void UENTArtificialIntelligenceSubsystem::RemoveAI(UObject* AIObject)
{
	if (!AIObject)
	{
		return;
	}

	if(AIObject->Implements<UENTArtificialIntelligenceInterface>())
	{
		AIList.Remove(AIObject);
	}
}

void UENTArtificialIntelligenceSubsystem::AddNoiseActorToBlackboard(UBlackboardComponent* BlackboardComponent, const FName& KeyName) const
{
	if (!BlackboardComponent)
	{
		return;
	}

	BlackboardComponent->SetValueAsObject(KeyName, NoiseActor);
}

void UENTArtificialIntelligenceSubsystem::UpdateNoiseActorLocation(const FVector& NewLocation) const
{
	if (!NoiseActor || bIsNoiseActorAttached)
	{
		return;
	}

	NoiseActor->SetActorLocation(NewLocation);
}

void UENTArtificialIntelligenceSubsystem::AttachNoiseActor(AActor* OtherActor)
{
	if (!NoiseActor || !OtherActor)
	{
		return;
	}

	NoiseActor->AttachToActor(OtherActor, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
	bIsNoiseActorAttached = true;
}

void UENTArtificialIntelligenceSubsystem::DetachNoiseActor()
{
	if (NoiseActor)
	{
		return;
	}

	NoiseActor->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	bIsNoiseActorAttached = false;
}

#pragma endregion
