// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/MoveToWithRotation.h"
#include "AIController.h"

UMoveToWithRotation::UMoveToWithRotation()
{
	bNotifyTick = true;

	NodeName = "MoveToWithRotation";
}

void UMoveToWithRotation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	USceneComponent* Root = OwnerComp.GetAIOwner()->GetPawn()->GetRootComponent();
	FRotator RootRotation = Root->GetComponentRotation();

	FVector RootVelocity = Root->GetComponentVelocity();
	FRotator VelocityRotator = FRotationMatrix::MakeFromX(RootVelocity).Rotator();

	RootRotation.Yaw = VelocityRotator.Yaw;
	Root->SetWorldRotation(RootRotation);
}
