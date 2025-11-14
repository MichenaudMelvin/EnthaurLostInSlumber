// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTFollowAIPath.h"
#include "AIController.h"
#include "ENTDefaultAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameplayTask.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Components/BoxComponent.h"
#include "Parasite/ENTParasitePawn.h"
#include "Path/ENTArtificialIntelligencePath.h"
#include "Tasks/AITask_MoveTo.h"

UENTFollowAIPath::UENTFollowAIPath()
{
	NodeName = "FollowAIPath";

	ForceInstancing(true);
	bNotifyTick = true;

	GroundObjects.Add(ObjectTypeQuery1);
	GroundObjects.Add(ObjectTypeQuery2);

	AIPath.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTFollowAIPath, AIPath), AENTArtificialIntelligencePath::StaticClass());
	PathIndex.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UENTFollowAIPath, PathIndex));
	WalkOnFloor.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UENTFollowAIPath, WalkOnFloor));
	TargetKeyLocation.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTFollowAIPath, TargetKeyLocation));
}

void UENTFollowAIPath::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		AIPath.ResolveSelectedKey(*BBAsset);
		PathIndex.ResolveSelectedKey(*BBAsset);
		WalkOnFloor.ResolveSelectedKey(*BBAsset);
		TargetKeyLocation.ResolveSelectedKey(*BBAsset);
	}

	if (!MovementCurve)
	{
		return;
	}

	FOnTimelineFloat UpdateEvent;
	FOnTimelineEvent FinishEvent;
	UpdateEvent.BindDynamic(this, &UENTFollowAIPath::MovementUpdate);
	FinishEvent.BindDynamic(this, &UENTFollowAIPath::FinishTask);
	MovementTimeline.AddInterpFloat(MovementCurve, UpdateEvent);
	MovementTimeline.SetTimelineFinishedFunc(FinishEvent);
}

EBTNodeResult::Type UENTFollowAIPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	UObject* KeyObject = BlackboardComponent->GetValue<UBlackboardKeyType_Object>(AIPath.GetSelectedKeyID());
	if (!KeyObject)
	{
		return EBTNodeResult::Failed;
	}

	AENTArtificialIntelligencePath* Path = Cast<AENTArtificialIntelligencePath>(KeyObject);
	if (!Path)
	{
		return EBTNodeResult::Failed;
	}

	AENTDefaultAIController* Controller = Cast<AENTDefaultAIController>(OwnerComp.GetAIOwner());
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	CurrentPawn = Controller->GetPawn();
	if (!CurrentPawn)
	{
		return EBTNodeResult::Failed;
	}

	float PawnHeight = 0.0f;
	AENTParasitePawn* ParasitePawn = Cast<AENTParasitePawn>(CurrentPawn);
	if (ParasitePawn)
	{
		PawnHeight = ParasitePawn->GetCollisionComp()->GetUnscaledBoxExtent().Z;
	}

	int Index = BlackboardComponent->GetValue<UBlackboardKeyType_Int>(PathIndex.GetSelectedKeyID());
	TargetLocation = Path->GetPointLocation(Index, PawnHeight);

	bool bWalkOnFloor = BlackboardComponent->GetValue<UBlackboardKeyType_Bool>(WalkOnFloor.GetSelectedKeyID());

	if (bWalkOnFloor)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Vector>(TargetKeyLocation.GetSelectedKeyID(), TargetLocation);

#if WITH_EDITORONLY_DATA
		if (bDebugTask && CurrentPawn)
		{
			UKismetSystemLibrary::DrawDebugPoint(CurrentPawn, TargetLocation, DebugPointSize, FColor::Red, 10.0f);
		}
#endif
		return EBTNodeResult::Succeeded;
	}

	CurrentOwnerComp = &OwnerComp;

	if (ParasitePawn)
	{
		ParasitePawn->OverrideVelocity(true);
	}

	StartLocation = CurrentPawn->GetActorLocation();

	FVector ForwardDirection = UKismetMathLibrary::GetDirectionUnitVector(StartLocation, TargetLocation);

	FVector EndLocation = StartLocation;

	EndLocation += (Path->GetDirection() * (GroundTraceLength + Path->GetWallOffset()));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(CurrentPawn);

	FHitResult HitResult;

	EDrawDebugTrace::Type DrawDebugTrace = EDrawDebugTrace::None;

#if WITH_EDITORONLY_DATA
	if (bDebugTask)
	{
		DrawDebugTrace = EDrawDebugTrace::ForDuration;
	}
#endif

	UKismetSystemLibrary::LineTraceSingleForObjects(CurrentPawn, StartLocation, EndLocation, GroundObjects, false, ActorsToIgnore, DrawDebugTrace, HitResult, false, FLinearColor::Red, FLinearColor::Green, 5.0f);

	FRotator TargetRotation = FRotationMatrix::MakeFromZX(HitResult.ImpactNormal, ForwardDirection).Rotator();
	CurrentPawn->SetActorRotation(TargetRotation);

	float Distance = FVector::Dist(StartLocation, TargetLocation);
	float Speed = CurrentPawn->GetMovementComponent()->GetMaxSpeed();

	float Time = Distance/Speed;

	MovementTimeline.SetPlayRate(1/Time);
	MovementTimeline.PlayFromStart();

	return EBTNodeResult::InProgress;
}

void UENTFollowAIPath::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	bool bWalkOnFloor = BlackboardComponent->GetValue<UBlackboardKeyType_Bool>(WalkOnFloor.GetSelectedKeyID());

#if WITH_EDITORONLY_DATA
	if (bDebugTask && CurrentPawn)
	{
		UKismetSystemLibrary::DrawDebugPoint(CurrentPawn, StartLocation, DebugPointSize, FColor::Green, 0.0f);

		UKismetSystemLibrary::DrawDebugPoint(CurrentPawn, TargetLocation, DebugPointSize, FColor::Red, 0.0f);
	}
#endif

	if (bWalkOnFloor)
	{
		return;
	}
	else
	{
		MovementTimeline.TickTimeline(DeltaSeconds);
	}
}

#if WITH_EDITOR
FString UENTFollowAIPath::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (AIPath.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		KeyDesc = AIPath.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Follow %s"), *KeyDesc);
}
#endif

void UENTFollowAIPath::MovementUpdate(float Alpha)
{
	FVector LerpResult = UKismetMathLibrary::VLerp(StartLocation, TargetLocation, Alpha);
	CurrentPawn->SetActorLocation(LerpResult);
}

void UENTFollowAIPath::FinishTask()
{
	AENTParasitePawn* ParasitePawn = Cast<AENTParasitePawn>(CurrentPawn);
	if (ParasitePawn)
	{
		ParasitePawn->OverrideVelocity(false);
	}

	FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Succeeded);
}