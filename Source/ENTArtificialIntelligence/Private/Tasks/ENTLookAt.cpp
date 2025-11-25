// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTLookAt.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Rotator.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Parasite/ENTParasitePawn.h"

#if WITH_EDITORONLY_DATA
#include "Debug/ENTArrowActor.h"
#endif

UENTLookAt::UENTLookAt()
{
	NodeName = "LookAt";
	bNotifyTick = true;

	ForceInstancing(true);

	LookAtPosition.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTLookAt, LookAtPosition));
	LookAtPosition.AddRotatorFilter(this, GET_MEMBER_NAME_CHECKED(UENTLookAt, LookAtPosition));
	LookAtPosition.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTLookAt, LookAtPosition), AActor::StaticClass());
}

void UENTLookAt::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		LookAtPosition.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UENTLookAt::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CurrentBlackboard)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* CurrentPawn = Controller->GetPawn();
	if (!CurrentPawn)
	{
		return EBTNodeResult::Failed;
	}

	if(LookAtPosition.SelectedKeyType == UBlackboardKeyType_Rotator::StaticClass())
	{
		TargetRotation = CurrentBlackboard->GetValue<UBlackboardKeyType_Rotator>(LookAtPosition.GetSelectedKeyID()); 
	}
	else
	{
		FVector TargetLookAt;
		if(LookAtPosition.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			TargetLookAt = CurrentBlackboard->GetValue<UBlackboardKeyType_Vector>(LookAtPosition.GetSelectedKeyID());
		}
		else if(LookAtPosition.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			const AActor* ActorToLookAt = Cast<AActor>(CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(LookAtPosition.GetSelectedKeyID()));

			if (!ActorToLookAt)
			{
				return EBTNodeResult::Failed;
			}

			TargetLookAt = ActorToLookAt->GetActorLocation();
		}

		FVector Direction = TargetLookAt - CurrentPawn->GetActorLocation();

		FVector UpVector = CurrentPawn->GetActorUpVector();
		AENTParasitePawn* ParasitePawn = Cast<AENTParasitePawn>(CurrentPawn);
		if (ParasitePawn)
		{
			UpVector = ParasitePawn->GetParasiteUpVector();
		}

		TargetRotation = FRotationMatrix::MakeFromZX(UpVector, Direction).Rotator();

#if WITH_EDITORONLY_DATA
		if (bDebugTask)
		{
			UKismetSystemLibrary::DrawDebugLine(this, CurrentPawn->GetActorLocation(), TargetLookAt, FLinearColor::Blue, 15.0f, 2.0f);

			UWorld* World = CurrentPawn->GetWorld();
			AENTArrowActor* ArrowActor = World->SpawnActor<AENTArrowActor>();

			if (ArrowActor)
			{
				ArrowActor->SetActorLocation(CurrentPawn->GetActorLocation());
				ArrowActor->SetArrowDirection(Direction);
				ArrowActor->SetArrowDimensions(5.0f, 50.0f);
			}
		}
#endif
	}

	if(bIgnorePitchRotation)
	{
		TargetRotation.Pitch = 0;
	}

	FQuat Quat = TargetRotation.Quaternion() * RotationOffset.Quaternion();
	TargetRotation = Quat.Rotator();

	if (bInstantRotation)
	{
		if (bUseControlRotation)
		{
			Controller->SetControlRotation(TargetRotation);
		}
		else
		{
			CurrentPawn->SetActorRotation(TargetRotation);
		}

		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::InProgress;
	}
}

void UENTLookAt::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (bInstantRotation)
	{
		return;
	}

	const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CurrentBlackboard)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* CurrentPawn = Controller->GetPawn();
	if (!CurrentPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FRotator CurrentRotation = bUseControlRotation ? Controller->GetControlRotation() : CurrentPawn->GetActorRotation();

	float Alpha = DeltaSeconds * RotationSpeed;

	FRotator ResultRotation = UKismetMathLibrary::RLerp(CurrentRotation, TargetRotation, Alpha, true);

	if (bUseControlRotation)
	{
		Controller->SetControlRotation(ResultRotation);
	}
	else
	{
		CurrentPawn->SetActorRotation(ResultRotation);
	}

	if (!TargetRotation.Equals(ResultRotation, RotationTolerance))
	{
		return;
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

#if WITH_EDITOR
FString UENTLookAt::GetStaticDescription() const
{
	FString LookAtPositionKeyDesc("invalid");
	if (LookAtPosition.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() || LookAtPosition.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass() || LookAtPosition.SelectedKeyType == UBlackboardKeyType_Rotator::StaticClass())
	{
		LookAtPositionKeyDesc = LookAtPosition.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Look at %s"), *LookAtPositionKeyDesc);
}
#endif
