// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ResetIgnoredCollisions.h"
#include "AIController.h"
#include "GameFramework/PawnMovementComponent.h"

EBTNodeResult::Type UResetIgnoredCollisions::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* CurrentPawn = Controller->GetPawn();
	if (!CurrentPawn)
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

	PrimitiveComponent->ClearMoveIgnoreActors();

	return EBTNodeResult::Succeeded;
}
