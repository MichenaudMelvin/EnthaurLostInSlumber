// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTLerpRotation.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

UENTLerpRotation::UENTLerpRotation()
{
	NodeName = "LerpRotation";
	bNotifyTick = true;
}

EBTNodeResult::Type UENTLerpRotation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	return EBTNodeResult::InProgress;
}

void UENTLerpRotation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* CurrentPawn = Controller->GetPawn();
	if (!CurrentPawn)
	{
		return;
	}

	FRotator CurrentRotation = CurrentPawn->GetActorRotation();
	float Alpha = DeltaSeconds * LerpSpeed.GetValue(OwnerComp);
	FRotator ResultRotation = UKismetMathLibrary::RLerp(CurrentRotation, TargetRotation.GetValue(OwnerComp), Alpha, true);
	CurrentPawn->SetActorRotation(ResultRotation);

	if (CurrentRotation.Equals(ResultRotation, Tolerance.GetValue(OwnerComp)))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
