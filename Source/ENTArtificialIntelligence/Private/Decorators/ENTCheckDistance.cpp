// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorators/ENTCheckDistance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/KismetSystemLibrary.h"

UENTCheckDistance::UENTCheckDistance()
{
	NodeName = "CheckDistance";
	Actor.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTCheckDistance, Actor), AActor::StaticClass());
	bNotifyTick = true;
	bNotifyProcessed = true;

	ForceInstancing(true);

	ObjectTypes.Add(ObjectTypeQuery1);
	ObjectTypes.Add(ObjectTypeQuery2);
}

void UENTCheckDistance::OnNodeProcessed(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult)
{
	Super::OnNodeProcessed(SearchData, NodeResult);

	FTimespan ElapsedTime;
	if (bHasTriggerAlreadyOnce)
	{
		ElapsedTime = FDateTime::Now() - LastTimeTriggered;
	}
	else
	{
		ElapsedTime = 0.0f;
	}

	bool bDistanceResult = ComputeDistance(SearchData.OwnerComp);

	if (DoCollisionTest(SearchData.OwnerComp))
	{
		bCollisionTestResult = TraceCollisionTest(SearchData.OwnerComp);
	}
	else
	{
		bCollisionTestResult = true;
	}

	ComputeSucceedDuration(ElapsedTime.GetSeconds(), bCollisionTestResult && bDistanceResult);
}

void UENTCheckDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	bool bDistanceResult = ComputeDistance(OwnerComp);

	if (DoCollisionTest(OwnerComp))
	{
		bCollisionTestResult = TraceCollisionTest(OwnerComp);
	}
	else
	{
		bCollisionTestResult = true;
	}

	ComputeSucceedDuration(DeltaSeconds, bCollisionTestResult && bDistanceResult);
	CheckAbort(OwnerComp, NodeMemory);
}

void UENTCheckDistance::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		Actor.ResolveSelectedKey(*BBAsset);
	}
}

void UENTCheckDistance::CheckAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(FlowAbortMode == EBTFlowAbortMode::None)
	{
		return;
	}

	const bool bIsOnActiveBranch = OwnerComp.IsExecutingBranch(GetMyNode(), GetChildIndex());

	bool bShouldAbort;

	if (bIsOnActiveBranch)
	{
		bShouldAbort = (FlowAbortMode == EBTFlowAbortMode::Self || FlowAbortMode == EBTFlowAbortMode::Both) && !CalculateRawConditionValue(OwnerComp, NodeMemory);
	}
	else
	{
		bShouldAbort = (FlowAbortMode == EBTFlowAbortMode::LowerPriority || FlowAbortMode == EBTFlowAbortMode::Both) && CalculateRawConditionValue(OwnerComp, NodeMemory);
	}

	if(bShouldAbort)
	{
		OwnerComp.RequestExecution(this);
	}
}

bool UENTCheckDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
#if WITH_EDITORONLY_DATA
	if (bDebugDecorator)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf( TEXT("bCollisionTestResult: %s"), (bCollisionTestResult ? TEXT("true") : TEXT("false"))));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf( TEXT("bDurationResult: %s"), (bDurationResult ? TEXT("true") : TEXT("false"))));
	}
#endif

	if (SucceedDuration <= 0.0f)
	{
		bool bDistanceResult = ComputeDistance(OwnerComp);

		if (DoCollisionTest(OwnerComp))
		{
			bool bCollisionResult = TraceCollisionTest(OwnerComp);
			return bCollisionResult && bDistanceResult;
		}
		else
		{
			return bDistanceResult;
		}
	}

	return bDurationResult;
}

#if WITH_EDITOR
FString UENTCheckDistance::GetStaticDescription() const
{
	if (IsInversed())
	{
		return "Change the check method rather than inverting the decorator";
	}

	FString KeyDesc("invalid");
	if (Actor.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		KeyDesc = Actor.SelectedKeyName.ToString();
	}

	FString CheckMethodStr = "None";
	switch (CheckMethod)
	{
	case EENTCheckMethod::Equal:
		CheckMethodStr = "==";
		break;
	case EENTCheckMethod::NotEqual:
		CheckMethodStr = "!=";
		break;
	case EENTCheckMethod::GreaterThanOrEqualTo:
		CheckMethodStr = ">=";
		break;
	case EENTCheckMethod::LessOrEqual:
		CheckMethodStr = "<=";
		break;
	case EENTCheckMethod::GreaterThan:
		CheckMethodStr = ">";
		break;
	case EENTCheckMethod::LessThan:
		CheckMethodStr = "<";
		break;
	}

	return FString::Printf(TEXT("Is distance between OwnerPawn and %s is %s %s cm"), *KeyDesc, *CheckMethodStr, *Distance.ToString());
}
#endif

bool UENTCheckDistance::ComputeDistance(UBehaviorTreeComponent& OwnerComp) const
{
	const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CurrentBlackboard)
	{
		return false;
	}

	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return false;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return false;
	}

	const UObject* Object = CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(Actor.SelectedKeyName);
	if (!Object)
	{
		return false;
	}

	const AActor* TargetActor = Cast<AActor>(Object);
	if (!TargetActor)
	{
		return false;
	}

	const float DistanceToActor = Pawn->GetDistanceTo(TargetActor);

	const float DistanceCheck = Distance.GetValue(OwnerComp);

	bool bResult = false;

	switch (CheckMethod)
	{
	case EENTCheckMethod::Equal:
		bResult = DistanceToActor == DistanceCheck;
		break;
	case EENTCheckMethod::NotEqual:
		bResult = DistanceToActor != DistanceCheck;
		break;
	case EENTCheckMethod::GreaterThanOrEqualTo:
		bResult = DistanceToActor >= DistanceCheck;
		break;
	case EENTCheckMethod::LessOrEqual:
		bResult = DistanceToActor <= DistanceCheck;
		break;
	case EENTCheckMethod::GreaterThan:
		bResult = DistanceToActor > DistanceCheck;
		break;
	case EENTCheckMethod::LessThan:
		bResult = DistanceToActor < DistanceCheck;
		break;
	}

#if WITH_EDITORONLY_DATA
	if (bDebugDecorator)
	{
		UKismetSystemLibrary::DrawDebugCylinder(Pawn, Pawn->GetActorLocation(), Pawn->GetActorLocation(), DistanceCheck, 12, FLinearColor::Green, 0.0f, 5.0f);
	}
#endif

	return bResult;
}

bool UENTCheckDistance::DoCollisionTest(const UBehaviorTreeComponent& OwnerComp) const
{
	bool bDoCollisionTest = CollisionTest.GetValue(OwnerComp);
	if (bInverseCollisionTestValue)
	{
		bDoCollisionTest = !bDoCollisionTest;
	}

	return bDoCollisionTest;
}

bool UENTCheckDistance::TraceCollisionTest(UBehaviorTreeComponent& OwnerComp) const
{
	const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CurrentBlackboard)
	{
		return false;
	}

	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return false;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return false;
	}

	const UObject* Object = CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(Actor.SelectedKeyName);
	if (!Object)
	{
		return false;
	}

	const AActor* TargetActor = Cast<AActor>(Object);
	if (!TargetActor)
	{
		return false;
	}

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Pawn);

	EDrawDebugTrace::Type DrawDebugTrace = EDrawDebugTrace::None;

#if WITH_EDITORONLY_DATA
	if (bDebugDecorator)
	{
		DrawDebugTrace = EDrawDebugTrace::ForOneFrame;
	}
#endif

	FHitResult HitResult;
	return !UKismetSystemLibrary::LineTraceSingleForObjects(this, Pawn->GetActorLocation(), TargetActor->GetActorLocation(), ObjectTypes, false, ActorsToIgnore, DrawDebugTrace, HitResult, false);
}

void UENTCheckDistance::ComputeSucceedDuration(float DeltaTime, bool bSucceedTests)
{
	if (SucceedDuration <= 0.0f)
	{
		bDurationResult = bSucceedTests;
		return;
	}

#if WITH_EDITORONLY_DATA
	if (bDebugDecorator)
	{
		UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %f"), DeltaTime);
	}
#endif

	LastTimeTriggered = FDateTime::Now();
	bHasTriggerAlreadyOnce = true;

	SucceedTime += DeltaTime * (bSucceedTests ? 1 : -1);
	SucceedTime = FMath::Clamp(SucceedTime, 0.0f, SucceedDuration);

#if WITH_EDITORONLY_DATA
	if (bDebugDecorator)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString::Printf(TEXT("SucceedTime %f"), SucceedTime));
	}
#endif

	bool bDurationSucceed = SucceedTime >= SucceedDuration;
	bool bDurationFailed = SucceedTime <= 0.0f;

	if (bDurationSucceed)
	{
		if (bHasTimerAlreadySucceed)
		{
			return;
		}

		bHasTimerAlreadyFailed = false;
		bHasTimerAlreadySucceed = true;
		bDurationResult = true;
	}
	else if (bDurationFailed)
	{
		if (bHasTimerAlreadyFailed)
		{
			return;
		}

		bHasTimerAlreadyFailed = true;
		bHasTimerAlreadySucceed = false;
		bDurationResult = false;
	}
}
