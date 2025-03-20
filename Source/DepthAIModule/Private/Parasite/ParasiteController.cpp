// Fill out your copyright notice in the Description page of Project Settings.


#include "Parasite/ParasiteController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Hearing.h"

AParasiteController::AParasiteController()
{
	PrimaryActorTick.bCanEverTick = false;

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception");
	PerceptionComponent->SetDominantSense(UAISense_Hearing::StaticClass());
}

void AParasiteController::BeginPlay()
{
	Super::BeginPlay();

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AParasiteController::OnTargetPerceptionUpdated);
}

void AParasiteController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	FAISenseID HearingID = UAISense::GetSenseID(UAISense_Hearing::StaticClass());

	if (Stimulus.Type == HearingID)
	{
		OnHearTarget(Actor, Stimulus);
	}
}

void AParasiteController::OnHearTarget(AActor* Actor, const FAIStimulus& Stimulus)
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
	GetBlackboardComponent()->SetValueAsVector(MoveLocationKeyName, HitResult.Location);
}

