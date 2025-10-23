// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTJumpToNextPath.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Parasite/ENTParasitePawn.h"
#include "Path/ENTArtificialIntelligencePath.h"

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
}

EBTNodeResult::Type UENTJumpToNextPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	CurrentOwnerComp = OwnerComp;
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

#if WITH_EDITORONLY_DATA
	if (bDebugTask)
	{
		UWorld* World = CurrentPawn->GetWorld();
		AActor* ArrowActor = World->SpawnActor(AActor::StaticClass());
		ArrowActor->SetActorLabel("ArrowActor", true);

		UActorComponent* ActorComp = ArrowActor->AddComponentByClass(UArrowComponent::StaticClass(), false, FTransform::Identity, false);
		if (ActorComp)
		{
			UArrowComponent* ArrowComp = Cast<UArrowComponent>(ActorComp);
			if (ArrowComp)
			{
				ArrowActor->SetRootComponent(ArrowComp);
				ArrowComp->SetWorldTransform(TargetTransform);
				ArrowComp->SetHiddenInGame(false);
				ArrowComp->SetVisibility(true);
				ArrowComp->SetArrowSize(5.0f);
				ArrowComp->SetArrowLength(25.0f);
			}
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

void UENTJumpToNextPath::MovementUpdate(float Alpha)
{
	FTransform ResultTransform = UKismetMathLibrary::TLerp(StartTransform, TargetTransform, Alpha, ELerpInterpolationMode::QuatInterp);

	CurrentPawn->SetActorTransform(ResultTransform);
}

void UENTJumpToNextPath::FinishTask()
{
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

	CurrentBlackboardComponent->SetValue<UBlackboardKeyType_Bool>(DoesWalkOnFloor.GetSelectedKeyID(), Path->IsOnFloor());
	CurrentBlackboardComponent->SetValue<UBlackboardKeyType_Object>(AIPath.GetSelectedKeyID(), Path);
	CurrentBlackboardComponent->SetValue<UBlackboardKeyType_Object>(AINextPath.GetSelectedKeyID(), nullptr);

	FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Succeeded);
}
