// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTDestroyPawn.h"

#include "AIController.h"

UENTDestroyPawn::UENTDestroyPawn()
{
	NodeName = "DestroyPawn";
}

EBTNodeResult::Type UENTDestroyPawn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	Pawn->Destroy();
	return EBTNodeResult::Succeeded;
}

#if WITH_EDITOR
FString UENTDestroyPawn::GetStaticDescription() const
{
	return "Destroy pawn";
}
#endif
