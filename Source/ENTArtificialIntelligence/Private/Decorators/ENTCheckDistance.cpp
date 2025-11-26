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

	ComputeDistance(SearchData.OwnerComp);
	bool bSucceed = TraceCollisionTest(SearchData.OwnerComp);
	ComputeCollisionTestDuration(ElapsedTime.GetSeconds(), bSucceed);
}

void UENTCheckDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ComputeDistance(OwnerComp);
	bool bSucceed = TraceCollisionTest(OwnerComp);
	ComputeCollisionTestDuration(DeltaSeconds, bSucceed);
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
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf( TEXT("bDistanceResult: %s"), (bDistanceResult ? TEXT("true") : TEXT("false"))));
	}
#endif

	if (DoCollisionTest(OwnerComp))
	{
		return bCollisionTestResult && bDistanceResult;
	}

	return bDistanceResult;
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

void UENTCheckDistance::ComputeDistance(UBehaviorTreeComponent& OwnerComp)
{
	bDistanceResult = false;

	const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CurrentBlackboard)
	{
		return;
	}

	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		return;
	}

	const UObject* Object = CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(Actor.SelectedKeyName);
	if (!Object)
	{
		return;
	}

	const AActor* TargetActor = Cast<AActor>(Object);
	if (!TargetActor)
	{
		return;
	}

	const float DistanceToActor = Pawn->GetDistanceTo(TargetActor);

	const float DistanceCheck = Distance.GetValue(OwnerComp);

	switch (CheckMethod)
	{
	case EENTCheckMethod::Equal:
		bDistanceResult = DistanceToActor == DistanceCheck;
		break;
	case EENTCheckMethod::NotEqual:
		bDistanceResult = DistanceToActor != DistanceCheck;
		break;
	case EENTCheckMethod::GreaterThanOrEqualTo:
		bDistanceResult = DistanceToActor >= DistanceCheck;
		break;
	case EENTCheckMethod::LessOrEqual:
		bDistanceResult = DistanceToActor <= DistanceCheck;
		break;
	case EENTCheckMethod::GreaterThan:
		bDistanceResult = DistanceToActor > DistanceCheck;
		break;
	case EENTCheckMethod::LessThan:
		bDistanceResult = DistanceToActor < DistanceCheck;
		break;
	}

#if WITH_EDITORONLY_DATA
	if (bDebugDecorator)
	{
		UKismetSystemLibrary::DrawDebugCylinder(Pawn, Pawn->GetActorLocation(), Pawn->GetActorLocation(), DistanceCheck, 12, FLinearColor::Green, 0.0f, 5.0f);
	}
#endif
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
	if (!bDistanceResult)
	{
		return false;
	}

	if (!DoCollisionTest(OwnerComp))
	{
		return false;
	}

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

void UENTCheckDistance::ComputeCollisionTestDuration(float DeltaTime, bool bSucceedCollisionTest)
{
	if (CollisionTestDuration <= 0.0f)
	{
		bCollisionTestResult = bSucceedCollisionTest;
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

	CollisionTestTime += DeltaTime * (bSucceedCollisionTest ? 1 : -1);
	CollisionTestTime = FMath::Clamp(CollisionTestTime, 0.0f, CollisionTestDuration);

#if WITH_EDITORONLY_DATA
	if (bDebugDecorator)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString::Printf(TEXT("CollisionTestTime %f"), CollisionTestTime));
	}
#endif

	bool bCollisionDurationSucceed = CollisionTestTime >= CollisionTestDuration;
	bool bCollisionDurationFailed = CollisionTestTime <= 0.0f;

	if (bCollisionDurationSucceed)
	{
		if (bHasTimerAlreadySucceed)
		{
			return;
		}

		bHasTimerAlreadyFailed = false;
		bHasTimerAlreadySucceed = true;
		bCollisionTestResult = true;
	}
	else if (bCollisionDurationFailed)
	{
		if (bHasTimerAlreadyFailed)
		{
			return;
		}

		bHasTimerAlreadyFailed = true;
		bHasTimerAlreadySucceed = false;
		bCollisionTestResult = false;
	}
}
