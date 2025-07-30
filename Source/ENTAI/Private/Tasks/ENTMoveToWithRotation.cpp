// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTMoveToWithRotation.h"
#include "AIController.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UENTMoveToWithRotation::UENTMoveToWithRotation()
{
	bNotifyTick = true;

	NodeName = "MoveToWithRotation";
}

void UENTMoveToWithRotation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!Pawn || !Pawn->GetMovementComponent())
	{
		return;
	}

	FVector PawnLocation = Pawn->GetActorLocation();

	FVector EndLocation = PawnLocation;
	EndLocation.Z -= GroundTraceLength;

	TArray<AActor*> Actors;
	Actors.Add(Pawn);

	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingle(Pawn, PawnLocation, EndLocation, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Actors, EDrawDebugTrace::None, HitResult, true);

	FVector ForwardVector = Pawn->GetMovementComponent()->Velocity;
	FVector RightVector = UKismetMathLibrary::RotateAngleAxis(ForwardVector, 90.0f, HitResult.Normal);
	ForwardVector.Normalize();
	RightVector.Normalize();

	FRotator XZRotator = FRotationMatrix::MakeFromXZ(ForwardVector, HitResult.Normal).Rotator();
	FRotator YZRotator = FRotationMatrix::MakeFromYZ(RightVector, HitResult.Normal).Rotator();

	FRotator TargetRotation(YZRotator.Pitch, XZRotator.Yaw, XZRotator.Roll);

	Pawn->SetActorRotation(TargetRotation);
}
