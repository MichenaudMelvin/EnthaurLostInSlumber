// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTJumpTask.h"

#include "AIController.h"
#include "ENTGravityPawnMovement.h"
#include "ENTToolStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Parasite/ENTParasitePawn.h"
#include "Path/ENTJumpSpline.h"

#if WITH_EDITORONLY_DATA
#include "Debug/ENTArrowActor.h"
#endif


UENTJumpTask::UENTJumpTask()
{
	NodeName = "Jump";
	bNotifyTick = true;

	ForceInstancing(true);

	JumpLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTJumpTask, JumpLocationKey));
}

void UENTJumpTask::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		JumpLocationKey.ResolveSelectedKey(*BBAsset);
	}

	if (!JumpCurve)
	{
		return;
	}

	FOnTimelineFloat UpdateEvent;
	FOnTimelineEvent FinishEvent;
	UpdateEvent.BindDynamic(this, &UENTJumpTask::MovementUpdate);
	FinishEvent.BindDynamic(this, &UENTJumpTask::FinishJumpTimeline);
	JumpTimeline.AddInterpFloat(JumpCurve, UpdateEvent);
	JumpTimeline.SetTimelineFinishedFunc(FinishEvent);
	JumpTimeline.SetPlayRate(1 / JumpDuration);
}

#if WITH_EDITOR
FString UENTJumpTask::GetStaticDescription() const
{
	FString JumpLocationKeyDesc("invalid");
	if (JumpLocationKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		JumpLocationKeyDesc = JumpLocationKey.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Jump to %s"), *JumpLocationKeyDesc);
}
#endif

EBTNodeResult::Type UENTJumpTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	CurrentPawn = CurrentOwnerComp->GetAIOwner()->GetPawn();
	if (!CurrentPawn)
	{
		return EBTNodeResult::Failed;
	}

	StartTransform = CurrentPawn->GetActorTransform();

	SetTargetTransform();

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

	SetPawnAnimToTrigger(StartJumpAnim);

	float AnimLength = UENTToolStatics::GetAnimLength(StartJumpAnim);
	AnimLength += StartJumpInitialDelay;
	GetWorld()->GetTimerManager().SetTimer(StartJumpAnimTimerHandle, this, &UENTJumpTask::StartJump, 1.0f, false, AnimLength);

	return EBTNodeResult::InProgress;
}

void UENTJumpTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	JumpTimeline.TickTimeline(DeltaSeconds);
}

EBTNodeResult::Type UENTJumpTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	GetWorld()->GetTimerManager().ClearTimer(StartJumpAnimTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(LandingAnimEndDelayTimerHandle);

	JumpTimeline.Stop();

	if (CurrentPawn)
	{
		Cast<IENTPawnAIInterface>(CurrentPawn)->SetAnimToTrigger(nullptr);
	}

	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UENTJumpTask::SetCurrentAnim(float CurrentTaskDuration)
{
	if (CurrentTaskDuration < SecondAnimAlpha)
	{
		SetPawnAnimToTrigger(FirstHalfAnim);
	}
	else if (CurrentTaskDuration >= LandingAnimAlpha)
	{
		SetPawnAnimToTrigger(LandingAnim);
	}
	else if (CurrentTaskDuration >= SecondAnimAlpha)
	{
		SetPawnAnimToTrigger(SecondHalfAnim);
	}
}

void UENTJumpTask::StartJump()
{
	UENTGravityPawnMovement* GravityPawnMovement = Cast<UENTGravityPawnMovement>(CurrentPawn->GetMovementComponent());
	if (GravityPawnMovement)
	{
		GravityPawnMovement->SetGravityScale(0.0f);
	}

	JumpTimeline.PlayFromStart();
}

void UENTJumpTask::SetPawnAnimToTrigger(UAnimSequenceBase* Anim) const
{
	if (!CurrentPawn->Implements<UENTPawnAIInterface>())
	{
		return;
	}

	Cast<IENTPawnAIInterface>(CurrentPawn)->SetAnimToTrigger(Anim);
}

void UENTJumpTask::MovementUpdate(float Alpha)
{
	SetCurrentAnim(Alpha);

	FTransform ResultTransform;
	ResultTransform.SetLocation(JumpSpline->GetLocationAtAlpha(Alpha));

	FQuat TargetRotation = FQuat::Slerp(StartTransform.GetRotation(), TargetTransform.GetRotation(), Alpha);

	ResultTransform.SetRotation(TargetRotation);
	ResultTransform.SetScale3D(FVector::OneVector);

	CurrentPawn->SetActorTransform(ResultTransform);
}

void UENTJumpTask::FinishJumpTimeline()
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

	GetWorld()->GetTimerManager().SetTimer(LandingAnimEndDelayTimerHandle, this, &UENTJumpTask::FinishTask, 1.0f, false, LandingDelay);
}

void UENTJumpTask::FinishTask()
{
	SetPawnAnimToTrigger(nullptr);

	UENTGravityPawnMovement* GravityPawnMovement = Cast<UENTGravityPawnMovement>(CurrentPawn->GetMovementComponent());
	if (GravityPawnMovement)
	{
		GravityPawnMovement->ResetGravityScale();
	}

	FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Succeeded);
}

void UENTJumpTask::SetTargetTransform()
{
	TargetTransform.SetLocation(CurrentBlackboardComponent->GetValue<UBlackboardKeyType_Vector>(JumpLocationKey.GetSelectedKeyID()));

	FQuat TargetRotation = FRotationMatrix::MakeFromXZ(GetTargetForwardVector(), FVector::UpVector).ToQuat();

	TargetTransform.SetRotation(TargetRotation * RotationOffset.Quaternion());
	TargetTransform.SetScale3D(FVector::OneVector);

	float PawnHeight = 0.0f;
	AENTParasitePawn* ParasitePawn = Cast<AENTParasitePawn>(CurrentPawn);
	if (ParasitePawn)
	{
		PawnHeight = ParasitePawn->GetParasiteHalfHeight();
	}

	FVector TargetLocation = TargetTransform.GetLocation();
	TargetLocation.Z += PawnHeight;

	TargetTransform.SetLocation(TargetLocation);
}

FVector UENTJumpTask::GetTargetForwardVector() const
{
	FVector ForwardVector = TargetTransform.GetLocation() - StartTransform.GetLocation();
	ForwardVector.Z = 0.0f;

	ForwardVector.Normalize();

	return ForwardVector;
}
