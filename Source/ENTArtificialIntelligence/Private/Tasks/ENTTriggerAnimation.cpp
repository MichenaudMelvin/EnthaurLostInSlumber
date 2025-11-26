// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTTriggerAnimation.h"

#include "AIController.h"
#include "ENTToolStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Interfaces/ENTPawnAIInterface.h"

UENTTriggerAnimation::UENTTriggerAnimation()
{
	NodeName = "TriggerAnimation";
	bNotifyTick = true;

	ForceInstancing(true);

	FinishCurrentTaskKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UENTTriggerAnimation, FinishCurrentTaskKey));
}

void UENTTriggerAnimation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		FinishCurrentTaskKey.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UENTTriggerAnimation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	CurrentOwnerComp = &OwnerComp;

	AAIController* Controller = CurrentOwnerComp->GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	if (!Pawn->Implements<UENTPawnAIInterface>())
	{
		return EBTNodeResult::Failed;
	}

	CurrentPawn = Pawn;

	if (!AnimationToTrigger)
	{
		return EBTNodeResult::Failed;
	}

	Cast<IENTPawnAIInterface>(CurrentPawn)->SetAnimToTrigger(AnimationToTrigger);

	float AnimLength = UENTToolStatics::GetAnimLength(AnimationToTrigger);
	GetWorld()->GetTimerManager().SetTimer(AnimationTimerHandle, this, &UENTTriggerAnimation::FinishTask, 1.0f, false, AnimLength);

	return EBTNodeResult::InProgress;
}

void UENTTriggerAnimation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return;
	}

	bool bShouldFinishTask = Blackboard->GetValue<UBlackboardKeyType_Bool>(FinishCurrentTaskKey.GetSelectedKeyID());

	if (bShouldFinishTask)
	{
		FinishTask();
	}
}

EBTNodeResult::Type UENTTriggerAnimation::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	GetWorld()->GetTimerManager().ClearTimer(AnimationTimerHandle);

	if (CurrentPawn && bResetAnimationIfAborted)
	{
		Cast<IENTPawnAIInterface>(CurrentPawn)->SetAnimToTrigger(nullptr);
	}

	return Super::AbortTask(OwnerComp, NodeMemory);
}

#if WITH_EDITOR
FString UENTTriggerAnimation::GetStaticDescription() const
{
	FString AnimName("invalid");
	if (AnimationToTrigger)
	{
		AnimName = AnimationToTrigger.GetName();
	}

	return FString::Printf(TEXT("Trigger %s"), *AnimName);
}
#endif

void UENTTriggerAnimation::FinishTask()
{
	GetWorld()->GetTimerManager().ClearTimer(AnimationTimerHandle);

	if (CurrentPawn)
	{
		Cast<IENTPawnAIInterface>(CurrentPawn)->SetAnimToTrigger(nullptr);
	}

	UBlackboardComponent* Blackboard = CurrentOwnerComp->GetBlackboardComponent();
	if (Blackboard)
	{
		Blackboard->SetValue<UBlackboardKeyType_Bool>(FinishCurrentTaskKey.GetSelectedKeyID(), false);
	}

	FinishLatentTask(*CurrentOwnerComp, EBTNodeResult::Succeeded);
}
