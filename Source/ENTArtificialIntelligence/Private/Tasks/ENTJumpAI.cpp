// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTJumpAI.h"

#include "AIController.h"
#include "ENTGravityPawnMovement.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Parasite/ENTParasitePawn.h"
#include "Path/ENTArtificialIntelligencePath.h"
#include "Path/ENTJumpSpline.h"

#if WITH_EDITORONLY_DATA
#include "Debug/ENTArrowActor.h"
#endif

UENTJumpAI::UENTJumpAI()
{
	NodeName = "Jump";
	bNotifyTick = true;

	AIPath.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTJumpAI, AIPath), AENTArtificialIntelligencePath::StaticClass());
	PathDirection.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UENTJumpAI, PathDirection));
	DoesWalkOnFloor.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UENTJumpAI, DoesWalkOnFloor));
}

void UENTJumpAI::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		AIPath.ResolveSelectedKey(*BBAsset);
		PathDirection.ResolveSelectedKey(*BBAsset);
		DoesWalkOnFloor.ResolveSelectedKey(*BBAsset);
	}

	if (!JumpCurve)
	{
		return;
	}

	FOnTimelineFloat UpdateEvent;
	FOnTimelineEvent FinishEvent;
	UpdateEvent.BindDynamic(this, &UENTJumpAI::MovementUpdate);
	FinishEvent.BindDynamic(this, &UENTJumpAI::FinishTask);
	JumpTimeline.AddInterpFloat(JumpCurve, UpdateEvent);
	JumpTimeline.SetTimelineFinishedFunc(FinishEvent);
	JumpTimeline.SetPlayRate(1 / JumpDuration);
}

EBTNodeResult::Type UENTJumpAI::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	CurrentOwnerComp = &OwnerComp;
	if (!CurrentOwnerComp)
	{
		return EBTNodeResult::Failed;
	}

	CurrentBlackboardComponent = CurrentOwnerComp->GetBlackboardComponent();
	if (!CurrentBlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	UObject* AIPathObject = CurrentBlackboardComponent->GetValue<UBlackboardKeyType_Object>(AIPath.GetSelectedKeyID());
	if (!AIPathObject)
	{
		return EBTNodeResult::Failed;
	}

	AENTArtificialIntelligencePath* CurrentPath = Cast<AENTArtificialIntelligencePath>(AIPathObject);
	if (!CurrentPath)
	{
		return EBTNodeResult::Failed;
	}

	CurrentPawn = CurrentOwnerComp->GetAIOwner()->GetPawn();
	if (!CurrentPawn)
	{
		return EBTNodeResult::Failed;
	}

	int32 Direction = CurrentBlackboardComponent->GetValue<UBlackboardKeyType_Int>(PathDirection.GetSelectedKeyID());

	StartTransform = CurrentPawn->GetActorTransform();

	if (bJumpOnTheGround)
	{
		TargetTransform.SetLocation(CurrentPath->GetNavLinkLocation(Direction));
		TargetTransform.SetRotation(FQuat::Identity);
		TargetTransform.SetScale3D(FVector::OneVector);
	}
	else
	{
		TargetTransform = Direction == 1 ? CurrentPath->GetStartTransform() : CurrentPath->GetEndTransform();
	}

	float PawnHeight = 0.0f;
	AENTParasitePawn* ParasitePawn = Cast<AENTParasitePawn>(CurrentPawn);
	if (ParasitePawn)
	{
		PawnHeight = ParasitePawn->GetCollisionComp()->GetUnscaledBoxExtent().Z;
	}

	FVector HeightOffset = PawnHeight * (CurrentPath->GetDirection() * -1);
	FVector TargetLocation = TargetTransform.GetLocation();
	TargetLocation += HeightOffset;

	TargetTransform.SetLocation(TargetLocation);

	UWorld* World = CurrentPawn->GetWorld();
	JumpSpline = World->SpawnActor<AENTJumpSpline>();
	if (!JumpSpline)
	{
		return EBTNodeResult::Failed;
	}

	JumpSpline->InitSpline(StartTransform, TargetTransform);

#if WITH_EDITORONLY_DATA
	if (bDebugTask)
	{
		JumpSpline->ShowSpline(FLinearColor::Blue);

		AENTArrowActor* ArrowActor = World->SpawnActor<AENTArrowActor>();
		if (ArrowActor)
		{
			ArrowActor->SetArrowColor(FColor::Blue);
			ArrowActor->SetActorTransform(TargetTransform);
			ArrowActor->SetArrowDimensions(5.0f, 50.0f);
		}
	}
#endif

	JumpTimeline.PlayFromStart();
	return EBTNodeResult::InProgress;
}

void UENTJumpAI::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	JumpTimeline.TickTimeline(DeltaSeconds);
}

#if WITH_EDITOR
FString UENTJumpAI::GetStaticDescription() const
{
	FString NextPathKeyDesc("invalid");
	if (AIPath.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		NextPathKeyDesc = AIPath.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Jump to %s during %f seconds"), *NextPathKeyDesc, JumpDuration);
}
#endif

void UENTJumpAI::MovementUpdate(float Alpha)
{
	FTransform ResultTransform;
	ResultTransform.SetLocation(JumpSpline->GetLocationAtAlpha(Alpha));

	FQuat TargetRotation = FQuat::Slerp(StartTransform.GetRotation(), TargetTransform.GetRotation(), Alpha);

	ResultTransform.SetRotation(TargetRotation);
	ResultTransform.SetScale3D(FVector::OneVector);

	CurrentPawn->SetActorTransform(ResultTransform);
}

void UENTJumpAI::FinishTask()
{
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
