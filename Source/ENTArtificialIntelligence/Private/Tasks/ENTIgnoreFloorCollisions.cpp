// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTIgnoreFloorCollisions.h"
#include "AIController.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UENTIgnoreFloorCollisions::UENTIgnoreFloorCollisions()
{
	NodeName = "IgnoreFloorCollisions";
}

EBTNodeResult::Type UENTIgnoreFloorCollisions::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* CurrentPawn = Controller->GetPawn();
	if (!CurrentPawn)
	{
		return EBTNodeResult::Failed;
	}

	FVector StartLocation = CurrentPawn->GetActorLocation();
	FVector EndLocation = StartLocation;
	EndLocation -= (CurrentPawn->GetActorUpVector() * TraceLength.GetValue(OwnerComp));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(CurrentPawn);

	FHitResult HitResult;
	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(CurrentPawn, StartLocation, EndLocation, CheckObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, false);

	if (!bHit || !HitResult.GetActor())
	{
		return EBTNodeResult::Failed;
	}

	if (!CurrentPawn->GetMovementComponent())
	{
		return EBTNodeResult::Failed;
	}

	USceneComponent* UpdatedComponent = CurrentPawn->GetMovementComponent()->UpdatedComponent;
	if (!UpdatedComponent)
	{
		return EBTNodeResult::Failed;
	}

	UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent);
	if (!PrimitiveComponent)
	{
		return EBTNodeResult::Failed;
	}

	PrimitiveComponent->IgnoreActorWhenMoving(HitResult.GetActor(), true);

	return EBTNodeResult::Succeeded;
}
