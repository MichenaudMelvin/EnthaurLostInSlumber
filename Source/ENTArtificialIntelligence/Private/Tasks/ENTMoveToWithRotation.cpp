// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTMoveToWithRotation.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Parasite/ENTParasitePawn.h"

UENTMoveToWithRotation::UENTMoveToWithRotation()
{
	NodeName = "MoveToWithRotation";
	bNotifyTick = true;

	ForceInstancing(true);
}

EBTNodeResult::Type UENTMoveToWithRotation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CurrentIndex = -1;
	CurrentPath = nullptr;

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UENTMoveToWithRotation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!Pawn || !Pawn->GetMovementComponent())
	{
		return;
	}

	UPathFollowingComponent* PathComp = OwnerComp.GetAIOwner()->GetPathFollowingComponent();
	if (!PathComp)
	{
		return;
	}

	FVector PawnLocation = Pawn->GetActorLocation();

	FVector EndLocation = PawnLocation;

	// AENTParasitePawn* Parasite = Cast<AENTParasitePawn>(Pawn);
	// if (Parasite)
	// {
	// 	EndLocation -= Parasite->GetParasiteUpVector() * GroundTraceLength;
	// }
	// else
	// {
		EndLocation.Z -= GroundTraceLength;
	// }

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
	UKismetSystemLibrary::LineTraceSingle(Pawn, PawnLocation, EndLocation, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Actors, DrawDebugTrace, HitResult, false);

	int32 NextIndex = UAIBlueprintHelperLibrary::GetCurrentPathIndex(OwnerComp.GetAIOwner()) + 1;
	TArray<FVector> PathPoints = UAIBlueprintHelperLibrary::GetCurrentPathPoints(OwnerComp.GetAIOwner());

	FNavigationPath* Path = PathComp->GetPath().Get();
	if (Path != CurrentPath || NextIndex != CurrentIndex)
	{
		CurrentPath = Path;

		CurrentIndex = NextIndex;

		FVector NextPoint = FVector::ZeroVector;
		if (PathPoints.IsValidIndex(CurrentIndex))
		{
			NextPoint = PathPoints[CurrentIndex];
		}

		CurrentDirection = (NextPoint - HitResult.Location).GetSafeNormal();
	}

	float SlopePitch;
	float SlopeRoll;
	UKismetMathLibrary::GetSlopeDegreeAngles(FVector::RightVector, HitResult.Normal, FVector::UpVector, SlopePitch, SlopeRoll);

	SlopePitch *= -1;

	FVector ForwardVector = UKismetMathLibrary::RotateAngleAxis(FVector::ForwardVector, SlopePitch, FVector::RightVector);

	ForwardVector.Normalize();

	float DotResult = FVector::DotProduct(CurrentDirection, ForwardVector);
	float Angle = (180.0f)/UE_DOUBLE_PI * FMath::Acos(DotResult);

	Angle *= CurrentDirection.Y > 0.0f ? 1.0f : -1.0f;

	ForwardVector = UKismetMathLibrary::RotateAngleAxis(ForwardVector, Angle, HitResult.Normal);
	ForwardVector.Normalize();

#if WITH_EDITORONLY_DATA
	if (bDebugTask)
	{
		FVector RightVector = UKismetMathLibrary::RotateAngleAxis(ForwardVector, 90.0f, HitResult.Normal);
		RightVector.Normalize();

		FVector StartLocation = Pawn->GetActorLocation();
		FVector EndForwardLocation = StartLocation + (ForwardVector * LineLength);
		FVector EndRightLocation = StartLocation + (RightVector * LineLength);
		FVector EndUpLocation = StartLocation + (HitResult.Normal * LineLength);

		FVector EndDirection = StartLocation + (CurrentDirection * LineLength);

		for (const FVector& Point : PathPoints)
		{
			UKismetSystemLibrary::DrawDebugPoint(this, Point, 5.0f, FLinearColor::Red, 0.0f);
		}

		UKismetSystemLibrary::DrawDebugLine(this, StartLocation, EndForwardLocation, FLinearColor::Red, 0.0f, 5.0f);
		UKismetSystemLibrary::DrawDebugLine(this, StartLocation, EndRightLocation, FLinearColor::Green, 0.0f, 5.0f);
		UKismetSystemLibrary::DrawDebugLine(this, StartLocation, EndUpLocation, FLinearColor::Blue, 0.0f, 5.0f);

		UKismetSystemLibrary::DrawDebugLine(this, StartLocation, EndDirection, FLinearColor::White, 0.0f, 5.0f);
	}
#endif

	FQuat ResultRotation = FRotationMatrix::MakeFromZX(HitResult.Normal, ForwardVector).ToQuat();

	FQuat TargetRotation = ResultRotation * RotationOffset.Quaternion();

	if (bLerpRotation)
	{
		TargetRotation = FQuat::Slerp(Pawn->GetActorRotation().Quaternion(), TargetRotation, (DeltaSeconds * RotationSpeed));
	}

#if WITH_EDITORONLY_DATA
	if (bDisableRotation)
	{
		return;
	}
#endif

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
