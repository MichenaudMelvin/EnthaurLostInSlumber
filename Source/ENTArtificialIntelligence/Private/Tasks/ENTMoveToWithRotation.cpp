// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTMoveToWithRotation.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UENTMoveToWithRotation::UENTMoveToWithRotation()
{
	NodeName = "MoveToWithRotation";
	bNotifyTick = true;
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

	EDrawDebugTrace::Type DrawDebugTrace = EDrawDebugTrace::None;

#if WITH_EDITORONLY_DATA
if (bDebugTask)
{
	DrawDebugTrace = EDrawDebugTrace::ForOneFrame;
}
#endif

	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingle(Pawn, PawnLocation, EndLocation, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Actors, DrawDebugTrace, HitResult, true);

	FVector ForwardVector = Pawn->GetMovementComponent()->Velocity;

	if (ForwardVector.Equals(FVector::ZeroVector, 0.0f))
	{
		FVector TargetLocation;
		const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* KeyValue = CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
			AActor* TargetActor = Cast<AActor>(KeyValue);
			if (TargetActor)
			{
				TargetLocation = TargetActor->GetActorLocation();
			}
		}
		else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			TargetLocation = CurrentBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
		}

		ForwardVector = TargetLocation - Pawn->GetActorLocation();
	}

	FVector RightVector = UKismetMathLibrary::RotateAngleAxis(ForwardVector, 90.0f, HitResult.Normal);
	ForwardVector.Normalize();
	RightVector.Normalize();

	FRotator XZRotator = FRotationMatrix::MakeFromXZ(ForwardVector, HitResult.Normal).Rotator();
	FRotator YZRotator = FRotationMatrix::MakeFromYZ(RightVector, HitResult.Normal).Rotator();

	FRotator TargetRotation(YZRotator.Pitch, XZRotator.Yaw, XZRotator.Roll);

	Pawn->SetActorRotation(TargetRotation);
}

#if WITH_EDITOR
FString UENTMoveToWithRotation::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() || BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		KeyDesc = BlackboardKey.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Move to: %s"), *KeyDesc);
}
#endif
