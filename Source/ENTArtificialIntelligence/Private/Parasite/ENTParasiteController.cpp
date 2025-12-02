// Fill out your copyright notice in the Description page of Project Settings.


#include "Parasite/ENTParasiteController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Subsystems/ENTArtificialIntelligenceSubsystem.h"

AENTParasiteController::AENTParasiteController()
{
#if WITH_EDITORONLY_DATA
	PrimaryActorTick.bCanEverTick = true;
#else
	PrimaryActorTick.bCanEverTick = false;
#endif

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception");
	PerceptionComponent->SetDominantSense(UAISense_Hearing::StaticClass());
}

void AENTParasiteController::BeginPlay()
{
	Super::BeginPlay();

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AENTParasiteController::OnTargetPerceptionUpdated);
}

#if WITH_EDITORONLY_DATA
void AENTParasiteController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bDebugAI || !Blackboard)
	{
		return;
	}

	int PathIndexValue = Blackboard->GetValueAsInt(PathIndexKeyName);
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("%s: %d"), *PathIndexKeyName.ToString(), PathIndexValue));

	bool bWalkOnFloor = Blackboard->GetValueAsBool(WalkOnFloorKeyName);
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("%s: %s"), *WalkOnFloorKeyName.ToString(), (bWalkOnFloor ? TEXT("true") : TEXT("false"))));

	FVector MoveLocationValue = Blackboard->GetValueAsVector(MoveLocationKeyName);
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("%s: %s"), *MoveLocationKeyName.ToString(), *MoveLocationValue.ToString()));

	bool bHeardNoiseValue = Blackboard->GetValueAsBool(HeardNoiseKeyName);
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("%s: %s"), *WalkOnFloorKeyName.ToString(), (bHeardNoiseValue ? TEXT("true") : TEXT("false"))));

	UObject* NoiseLocationValue = Blackboard->GetValueAsObject(NoiseLocationKeyName);
	FString NoiseLocationValueName = "Nullptr";
	if (NoiseLocationValue)
	{
		NoiseLocationValueName = NoiseLocationValue->GetName();
	}
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("%s: %s"), *NoiseLocationKeyName.ToString(), *NoiseLocationValueName));

	UObject* NoiseInverstigator = Blackboard->GetValueAsObject(NoiseInvestigatorKeyName);
	FString NoiseInverstigatorName = "Nullptr";
	if (NoiseInverstigator)
	{
		NoiseInverstigatorName = NoiseInverstigator->GetName();
	}
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("%s: %s"), *NoiseInvestigatorKeyName.ToString(), *NoiseInverstigatorName));
}
#endif

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
	if (!Blackboard)
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

	Blackboard->SetValueAsBool(HeardNoiseKeyName, true);
	Blackboard->SetValueAsObject(NoiseInvestigatorKeyName, Actor);

	UENTArtificialIntelligenceSubsystem* AISubsystem = GetWorld()->GetSubsystem<UENTArtificialIntelligenceSubsystem>();
	if (AISubsystem)
	{
		AISubsystem->UpdateNoiseActorLocation(HitResult.Location);
		AISubsystem->AddNoiseActorToBlackboard(Blackboard, NoiseLocationKeyName);
	}
}

void AENTParasiteController::OnUnheardTarget(AActor* Actor)
{
	if (!Blackboard)
	{
		return;
	}

	UObject* ObjectPtr = Blackboard->GetValueAsObject(NoiseInvestigatorKeyName);

	if (ObjectPtr != Actor)
	{
		return;
	}

	Blackboard->SetValueAsBool(HeardNoiseKeyName, false);
	Blackboard->SetValueAsObject(NoiseLocationKeyName, nullptr);
	Blackboard->SetValueAsObject(NoiseInvestigatorKeyName, nullptr);
}

void AENTParasiteController::SaveControllerData(FENTAIData& AIData)
{
	Super::SaveControllerData(AIData);

	if (!Blackboard)
	{
		return;
	}

	FENTParasiteData& ParaSiteData = static_cast<FENTParasiteData&>(AIData);
	ParaSiteData.PathIndex = Blackboard->GetValueAsInt(PathIndexKeyName);
	ParaSiteData.PathDirection = Blackboard->GetValueAsInt(PathDirectionKeyName);
	ParaSiteData.bWalkOnFloor = Blackboard->GetValueAsBool(WalkOnFloorKeyName);
	ParaSiteData.MoveToLocation = Blackboard->GetValueAsVector(MoveLocationKeyName);
	ParaSiteData.bHeardNoise = Blackboard->GetValueAsBool(HeardNoiseKeyName);
}

void AENTParasiteController::LoadControllerData(const FENTAIData& AIData)
{
	Super::LoadControllerData(AIData);

	if (!Blackboard)
	{
		return;
	}

	const FENTParasiteData& ParaSiteData = static_cast<const FENTParasiteData&>(AIData);
	Blackboard->SetValueAsInt(PathIndexKeyName, ParaSiteData.PathIndex);
	Blackboard->SetValueAsInt(PathDirectionKeyName, ParaSiteData.PathDirection);
	Blackboard->SetValueAsBool(WalkOnFloorKeyName, ParaSiteData.bWalkOnFloor);
	Blackboard->SetValueAsVector(MoveLocationKeyName, ParaSiteData.MoveToLocation);
	Blackboard->SetValueAsBool(HeardNoiseKeyName, ParaSiteData.bHeardNoise);
}
