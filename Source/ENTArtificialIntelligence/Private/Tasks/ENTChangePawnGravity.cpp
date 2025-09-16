// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTChangePawnGravity.h"
#include "AIController.h"
#include "ENTGravityPawnMovement.h"

UENTChangePawnGravity::UENTChangePawnGravity()
{
	NodeName = "ChangePawnGravity";
}

EBTNodeResult::Type UENTChangePawnGravity::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	UENTGravityPawnMovement* GravityPawnMovement = CurrentPawn->GetComponentByClass<UENTGravityPawnMovement>();
	if (!GravityPawnMovement)
	{
		return EBTNodeResult::Failed;
	}

	GravityPawnMovement->SetGravityScale(NewGravityScale);
	GravityPawnMovement->SetGravityDirection(NewGravityDirection);

	return EBTNodeResult::Succeeded;
}
