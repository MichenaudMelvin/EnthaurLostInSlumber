// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ChangePawnGravity.h"
#include "AIController.h"
#include "Components/GravityPawnMovement.h"

EBTNodeResult::Type UChangePawnGravity::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CurrentBlackboard)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* CurrentPawn = Controller->GetPawn();
	if (!CurrentPawn)
	{
		return EBTNodeResult::Failed;
	}

	UGravityPawnMovement* GravityPawnMovement = CurrentPawn->GetComponentByClass<UGravityPawnMovement>();
	if (!GravityPawnMovement)
	{
		return EBTNodeResult::Failed;
	}

	GravityPawnMovement->SetGravityScale(NewGravityScale);
	GravityPawnMovement->SetGravityDirection(NewGravityDirection);

	return EBTNodeResult::Succeeded;
}
