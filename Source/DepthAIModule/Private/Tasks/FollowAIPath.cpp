// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/FollowAIPath.h"
#include "AIController.h"
#include "DefaultAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Parasite/ParasitePawn.h"
#include "Path/AIPath.h"

UFollowAIPath::UFollowAIPath()
{
	ForceInstancing(true);
	bNotifyTick = true;

	AIPath.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UFollowAIPath, AIPath), AAIPath::StaticClass());
	PathIndex.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UFollowAIPath, PathIndex));
	WalkOnFloor.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UFollowAIPath, WalkOnFloor));
}

void UFollowAIPath::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		AIPath.ResolveSelectedKey(*BBAsset);
		PathIndex.ResolveSelectedKey(*BBAsset);
		WalkOnFloor.ResolveSelectedKey(*BBAsset);
	}

	if (!MovementCurve)
	{
		return;
	}

	FOnTimelineFloat UpdateEvent;
	FOnTimelineEvent FinishEvent;
	UpdateEvent.BindDynamic(this, &UFollowAIPath::MovementUpdate);
	FinishEvent.BindDynamic(this, &UFollowAIPath::FinishTask);
	MovementTimeline.AddInterpFloat(MovementCurve, UpdateEvent);
	MovementTimeline.SetTimelineFinishedFunc(FinishEvent);
}

EBTNodeResult::Type UFollowAIPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	AAIPath* Path = Cast<AAIPath>(KeyObject);
	if (!Path)
	{
		return EBTNodeResult::Failed;
	}

	ADefaultAIController* Controller = Cast<ADefaultAIController>(OwnerComp.GetAIOwner());
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
	AParasitePawn* ParasitePawn = Cast<AParasitePawn>(CurrentPawn);
	if (ParasitePawn)
	{
		PawnHeight = ParasitePawn->GetCollisionComp()->GetUnscaledBoxExtent().Z;
	}

	int Index = BlackboardComponent->GetValue<UBlackboardKeyType_Int>(PathIndex.GetSelectedKeyID());
	TargetLocation = Path->GetPointLocation(Index, PawnHeight);

	bool bWalkOnFloor = BlackboardComponent->GetValue<UBlackboardKeyType_Bool>(WalkOnFloor.GetSelectedKeyID());

	if (bWalkOnFloor)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerComp.GetAIOwner(), TargetLocation);
	}
	else
	{
		CurrentOwnerComp = &OwnerComp;
		StartLocation = CurrentPawn->GetActorLocation();

		float Distance = FVector::Dist(StartLocation, TargetLocation);
		float Speed = CurrentPawn->GetMovementComponent()->GetMaxSpeed();

		float Time = Distance/Speed;

		MovementTimeline.SetPlayRate(1/Time);
		MovementTimeline.PlayFromStart();
	}

	return EBTNodeResult::InProgress;
}

void UFollowAIPath::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();

	if(!Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	bool bWalkOnFloor = BlackboardComponent->GetValue<UBlackboardKeyType_Bool>(WalkOnFloor.GetSelectedKeyID());

	if (!bWalkOnFloor)
	{
		MovementTimeline.TickTimeline(DeltaSeconds);
		return;
	}

	if (bRotateWithMovement)
	{
		USceneComponent* Root = OwnerComp.GetAIOwner()->GetPawn()->GetRootComponent();
		FRotator RootRotation = Root->GetComponentRotation();

		FVector RootVelocity = Root->GetComponentVelocity();
		FRotator VelocityRotator = FRotationMatrix::MakeFromX(RootVelocity).Rotator();

		RootRotation.Yaw = VelocityRotator.Yaw;
		Root->SetWorldRotation(RootRotation);
	}

	FVector PawnLocation = Pawn->GetActorLocation();

	if (PawnLocation.Equals(TargetLocation, Tolerance))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UFollowAIPath::MovementUpdate(float Alpha)
{
	FVector LerpResult = UKismetMathLibrary::VLerp(StartLocation, TargetLocation, Alpha);
	CurrentPawn->SetActorLocation(LerpResult);
}

void UFollowAIPath::FinishTask()
{
	FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Succeeded);
}