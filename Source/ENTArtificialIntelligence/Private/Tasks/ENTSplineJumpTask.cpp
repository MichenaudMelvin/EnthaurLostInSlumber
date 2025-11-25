// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTSplineJumpTask.h"

#include "ENTGravityPawnMovement.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Components/BoxComponent.h"
#include "Parasite/ENTParasitePawn.h"
#include "Path/ENTArtificialIntelligencePath.h"
#include "Path/ENTJumpSpline.h"

UENTSplineJumpTask::UENTSplineJumpTask()
{
	NodeName = "SplineJump";

	ForceInstancing(true);

	AIPath.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTSplineJumpTask, AIPath), AENTArtificialIntelligencePath::StaticClass());
	PathDirection.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UENTSplineJumpTask, PathDirection));
	DoesWalkOnFloor.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UENTSplineJumpTask, DoesWalkOnFloor));
}

void UENTSplineJumpTask::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		AIPath.ResolveSelectedKey(*BBAsset);
		PathDirection.ResolveSelectedKey(*BBAsset);
		DoesWalkOnFloor.ResolveSelectedKey(*BBAsset);
	}
}

#if WITH_EDITOR
FString UENTSplineJumpTask::GetStaticDescription() const
{
	FString NextPathKeyDesc("invalid");
	if (AIPath.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		NextPathKeyDesc = AIPath.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Jump to %s during %f seconds"), *NextPathKeyDesc, JumpDuration);
}
#endif


void UENTSplineJumpTask::SetTargetTransform()
{
	// Super::SetTargetTransform();

	UObject* AIPathObject = CurrentBlackboardComponent->GetValue<UBlackboardKeyType_Object>(AIPath.GetSelectedKeyID());
	if (!AIPathObject)
	{
		return;
	}

	AENTArtificialIntelligencePath* CurrentPath = Cast<AENTArtificialIntelligencePath>(AIPathObject);
	if (!CurrentPath)
	{
		return;
	}

	int32 Direction = CurrentBlackboardComponent->GetValue<UBlackboardKeyType_Int>(PathDirection.GetSelectedKeyID());

	StartTransform = CurrentPawn->GetActorTransform();

	if (bJumpOnTheGround)
	{
		TargetTransform.SetLocation(CurrentPath->GetNavLinkLocation(Direction));

		FQuat TargetRotation = FRotationMatrix::MakeFromXZ(GetTargetForwardVector(), FVector::UpVector).ToQuat();
		TargetTransform.SetRotation(TargetRotation);
		TargetTransform.SetScale3D(FVector::OneVector);
	}
	else
	{
		TargetTransform = Direction == 1 ? CurrentPath->GetStartTransform(Direction) : CurrentPath->GetEndTransform(Direction);
	}

	TargetTransform.SetRotation(TargetTransform.GetRotation() * RotationOffset.Quaternion());

	float PawnHeight = 0.0f;
	AENTParasitePawn* ParasitePawn = Cast<AENTParasitePawn>(CurrentPawn);
	if (ParasitePawn)
	{
		PawnHeight = ParasitePawn->GetParasiteHalfHeight();
	}

	FVector GroundDirection = bJumpOnTheGround ? FVector::UpVector : (CurrentPath->GetDirection() * -1);
	FVector HeightOffset = PawnHeight * GroundDirection;
	FVector TargetLocation = TargetTransform.GetLocation();
	TargetLocation += HeightOffset;

	TargetTransform.SetLocation(TargetLocation);
}

void UENTSplineJumpTask::FinishTask()
{
	// Super::FinishTask();

	if (JumpSpline)
	{
#if WITH_EDITORONLY_DATA
		if (!bDebugTask)
		{
#endif
		JumpSpline->Destroy();
#if WITH_EDITORONLY_DATA
		}
#endif
	}

	UObject* PathObject = CurrentBlackboardComponent->GetValue<UBlackboardKeyType_Object>(AIPath.GetSelectedKeyID());

	if (!PathObject)
	{
		FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Failed);
		return;
	}

	AENTArtificialIntelligencePath* Path = Cast<AENTArtificialIntelligencePath>(PathObject);
	if (!Path)
	{
		FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Failed);
		return;
	}

	UENTGravityPawnMovement* GravityPawnMovement = Cast<UENTGravityPawnMovement>(CurrentPawn->GetMovementComponent());
	if (GravityPawnMovement)
	{
		(Path->IsOnFloor() || bJumpOnTheGround) ? GravityPawnMovement->ResetGravityScale() : GravityPawnMovement->SetGravityScale(0.0f);
	}

	CurrentBlackboardComponent->SetValue<UBlackboardKeyType_Bool>(DoesWalkOnFloor.GetSelectedKeyID(), (Path->IsOnFloor() || bJumpOnTheGround));

	FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Succeeded);
}
