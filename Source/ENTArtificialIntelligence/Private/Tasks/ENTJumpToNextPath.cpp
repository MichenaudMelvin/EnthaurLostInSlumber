// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTJumpToNextPath.h"

#include "AIController.h"
#include "ENTGravityPawnMovement.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Parasite/ENTParasitePawn.h"
#include "Path/ENTArtificialIntelligencePath.h"
#include "Path/ENTJumpSpline.h"

#if WITH_EDITORONLY_DATA
#include "Debug/ENTArrowActor.h"
#endif

UENTJumpToNextPath::UENTJumpToNextPath()
{
	NodeName = "JumpToNextPath";
	bNotifyTick = true;

	AIPath.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTJumpToNextPath, AIPath), AENTArtificialIntelligencePath::StaticClass());
	AINextPath.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTJumpToNextPath, AINextPath),  AENTArtificialIntelligencePath::StaticClass());
	DoesWalkOnFloor.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UENTJumpToNextPath, DoesWalkOnFloor));
}

void UENTJumpToNextPath::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		AIPath.ResolveSelectedKey(*BBAsset);
		AINextPath.ResolveSelectedKey(*BBAsset);
		DoesWalkOnFloor.ResolveSelectedKey(*BBAsset);
	}

	if (!JumpCurve)
	{
		return;
	}

	FOnTimelineFloat UpdateEvent;
	FOnTimelineEvent FinishEvent;
	UpdateEvent.BindDynamic(this, &UENTJumpToNextPath::MovementUpdate);
	FinishEvent.BindDynamic(this, &UENTJumpToNextPath::FinishTask);
	JumpTimeline.AddInterpFloat(JumpCurve, UpdateEvent);
	JumpTimeline.SetTimelineFinishedFunc(FinishEvent);
	JumpTimeline.SetPlayRate(1 / JumpDuration);
}

EBTNodeResult::Type UENTJumpToNextPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	UObject* AINextPathObject = CurrentBlackboardComponent->GetValue<UBlackboardKeyType_Object>(AINextPath.GetSelectedKeyID());

	if (!AIPathObject || !AINextPathObject)
	{
		return EBTNodeResult::Failed;
	}

	AENTArtificialIntelligencePath* CurrentPath = Cast<AENTArtificialIntelligencePath>(AIPathObject);
	AENTArtificialIntelligencePath* NextPath = Cast<AENTArtificialIntelligencePath>(AINextPathObject);

	if (!CurrentPath || !NextPath)
	{
		return EBTNodeResult::Failed;
	}

	CurrentPawn = CurrentOwnerComp->GetAIOwner()->GetPawn();
	if (!CurrentPawn)
	{
		return EBTNodeResult::Failed;
	}

	StartTransform = CurrentPawn->GetActorTransform();
	TargetTransform = NextPath->GetStartTransform();

	float PawnHeight = 0.0f;
	AENTParasitePawn* ParasitePawn = Cast<AENTParasitePawn>(CurrentPawn);
	if (ParasitePawn)
	{
		PawnHeight = ParasitePawn->GetCollisionComp()->GetUnscaledBoxExtent().Z;
	}

	FVector HeightOffset = PawnHeight * (NextPath->GetDirection() * -1);
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

void UENTJumpToNextPath::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	JumpTimeline.TickTimeline(DeltaSeconds);
}

#if WITH_EDITOR
FString UENTJumpToNextPath::GetStaticDescription() const
{
	FString AIPathKeyDesc("invalid");
	if (AIPath.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		AIPathKeyDesc = AIPath.SelectedKeyName.ToString();
	}

	FString NextPathKeyDesc("invalid");
	if (AINextPath.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		NextPathKeyDesc = AINextPath.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Jump from %s to %s during %f seconds"), *AIPathKeyDesc, *NextPathKeyDesc, JumpDuration);
}
#endif

void UENTJumpToNextPath::MovementUpdate(float Alpha)
{
	FTransform ResultTransform;
	ResultTransform.SetLocation(JumpSpline->GetLocationAtAlpha(Alpha));

	FQuat TargetRotation = FQuat::Slerp(StartTransform.GetRotation(), TargetTransform.GetRotation(), Alpha);

	ResultTransform.SetRotation(TargetRotation);
	ResultTransform.SetScale3D(FVector::OneVector);

	CurrentPawn->SetActorTransform(ResultTransform);
}

void UENTJumpToNextPath::FinishTask()
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

	UObject* NextAIPath = CurrentBlackboardComponent->GetValue<UBlackboardKeyType_Object>(AINextPath.GetSelectedKeyID());

	if (!NextAIPath)
	{
		FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Failed);
		return;
	}

	AENTArtificialIntelligencePath* Path = Cast<AENTArtificialIntelligencePath>(NextAIPath);
	if (!Path)
	{
		FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Failed);
		return;
	}

	UENTGravityPawnMovement* GravityPawnMovement = Cast<UENTGravityPawnMovement>(CurrentPawn->GetMovementComponent());
	if (GravityPawnMovement)
	{
		Path->IsOnFloor() ? GravityPawnMovement->ResetGravityScale() : GravityPawnMovement->SetGravityScale(0.0f);
	}

	CurrentBlackboardComponent->SetValue<UBlackboardKeyType_Bool>(DoesWalkOnFloor.GetSelectedKeyID(), Path->IsOnFloor());
	CurrentBlackboardComponent->SetValue<UBlackboardKeyType_Object>(AIPath.GetSelectedKeyID(), Path);
	CurrentBlackboardComponent->SetValue<UBlackboardKeyType_Object>(AINextPath.GetSelectedKeyID(), nullptr);

	FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Succeeded);
}
