// Fill out your copyright notice in the Description page of Project Settings.


#include "Parasite/ENTParasiteController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Saves/WorldSaves/ENTGameElementData.h"

AENTParasiteController::AENTParasiteController()
{
	PrimaryActorTick.bCanEverTick = false;

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception");
	PerceptionComponent->SetDominantSense(UAISense_Hearing::StaticClass());
}

void AENTParasiteController::BeginPlay()
{
	Super::BeginPlay();

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AENTParasiteController::OnTargetPerceptionUpdated);
}

void AENTParasiteController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!PerceptionComponent)
	{
		return;
	}

	if (PerceptionComponent->OnTargetPerceptionUpdated.IsAlreadyBound(this, &AENTParasiteController::OnTargetPerceptionUpdated))
	{
		PerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AENTParasiteController::OnTargetPerceptionUpdated);
	}
}

void AENTParasiteController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	FAISenseID HearingID = UAISense::GetSenseID(UAISense_Hearing::StaticClass());

	if (Stimulus.Type == HearingID)
	{
		if (Stimulus.IsExpired())
		{
			OnUnheardTarget(Actor);
		}
		else
		{
			OnHearTarget(Actor, Stimulus);
		}
	}
}

void AENTParasiteController::OnHearTarget(AActor* Actor, const FAIStimulus& Stimulus)
{
	if (!GetBlackboardComponent())
	{
		return;
	}

	FHitResult HitResult;
	FVector StartLocation = Stimulus.StimulusLocation;
	FVector EndLocation = StartLocation;
	EndLocation.Z -= HearingZTolerance;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility);

	if (!bHit)
	{
		return;
	}

	GetBlackboardComponent()->SetValueAsBool(HeardNoiseKeyName, true);
	GetBlackboardComponent()->SetValueAsObject(NoiseInvestigatorKeyName, Actor);
}

void AENTParasiteController::OnUnheardTarget(AActor* Actor)
{
	if (!GetBlackboardComponent())
	{
		return;
	}

	UObject* ObjectPtr = GetBlackboardComponent()->GetValueAsObject(NoiseInvestigatorKeyName);

	if (ObjectPtr != Actor)
	{
		return;
	}

	GetBlackboardComponent()->SetValueAsBool(HeardNoiseKeyName, false);
	GetBlackboardComponent()->SetValueAsObject(NoiseInvestigatorKeyName, nullptr);
}

void AENTParasiteController::SaveControllerData(FENTAIData& AIData)
{
	Super::SaveControllerData(AIData);

	if (!GetBlackboardComponent())
	{
		return;
	}

	FENTParasiteData& ParaSiteData = static_cast<FENTParasiteData&>(AIData);
	ParaSiteData.PathIndex = GetBlackboardComponent()->GetValueAsInt(PathIndexKeyName);
	ParaSiteData.PathDirection = GetBlackboardComponent()->GetValueAsInt(PathDirectionKeyName);
	ParaSiteData.bWalkOnFloor = GetBlackboardComponent()->GetValueAsBool(WalkOnFloorKeyName);
	ParaSiteData.MoveToLocation = GetBlackboardComponent()->GetValueAsVector(MoveLocationKeyName);
	ParaSiteData.bHeardNoise = GetBlackboardComponent()->GetValueAsBool(HeardNoiseKeyName);
}

void AENTParasiteController::LoadControllerData(const FENTAIData& AIData)
{
	Super::LoadControllerData(AIData);

	if (!GetBlackboardComponent())
	{
		return;
	}

	const FENTParasiteData& ParaSiteData = static_cast<const FENTParasiteData&>(AIData);
	GetBlackboardComponent()->SetValueAsInt(PathIndexKeyName, ParaSiteData.PathIndex);
	GetBlackboardComponent()->SetValueAsInt(PathDirectionKeyName, ParaSiteData.PathDirection);
	GetBlackboardComponent()->SetValueAsBool(WalkOnFloorKeyName, ParaSiteData.bWalkOnFloor);
	GetBlackboardComponent()->SetValueAsVector(MoveLocationKeyName, ParaSiteData.MoveToLocation);
	GetBlackboardComponent()->SetValueAsBool(HeardNoiseKeyName, ParaSiteData.bHeardNoise);
}
