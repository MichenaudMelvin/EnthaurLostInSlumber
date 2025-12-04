// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/TriggerNoiseAtLocation.h"

#include "AkGameplayStatics.h"
#include "ENTDefaultAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UTriggerNoiseAtLocation::UTriggerNoiseAtLocation()
{
	NodeName = "TriggerNoiseAtLocation";

	ForceInstancing(true);
	NoiseLocation.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UTriggerNoiseAtLocation, NoiseLocation), AActor::StaticClass());
	NoiseLocation.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UTriggerNoiseAtLocation, NoiseLocation));
}

void UTriggerNoiseAtLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		NoiseLocation.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UTriggerNoiseAtLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	AENTDefaultAIController* Controller = Cast<AENTDefaultAIController>(OwnerComp.GetAIOwner());
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	APawn* CurrentPawn = Controller->GetPawn();
	if (!CurrentPawn)
	{
		return EBTNodeResult::Failed;
	}

	FVector Location = FVector::ZeroVector;
	if (NoiseLocation.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* Object = BlackboardComponent->GetValue<UBlackboardKeyType_Object>(NoiseLocation.GetSelectedKeyID());
		if (Object)
		{
			AActor* Actor = Cast<AActor>(Object);
			if (Actor)
			{
				Location = Actor->GetActorLocation();
			}
		}
	}
	else if (NoiseLocation.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		Location = BlackboardComponent->GetValue<UBlackboardKeyType_Vector>(NoiseLocation.GetSelectedKeyID());
	}

	CurrentPawn->MakeNoise(NoiseLoudness, CurrentPawn, Location, NoiseRange, NoiseTag);

	if (AudioEvent)
	{
		UAkGameplayStatics::PostEventAtLocation(AudioEvent, Location, FRotator::ZeroRotator, CurrentPawn);
	}

	return EBTNodeResult::Succeeded;
}

#if WITH_EDITOR
FString UTriggerNoiseAtLocation::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (NoiseLocation.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() || NoiseLocation.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		KeyDesc = NoiseLocation.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Trigger noise at %s location"), *KeyDesc);
}
#endif
