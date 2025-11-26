// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/ENTTraceLocationToGround.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/KismetSystemLibrary.h"

UENTTraceLocationToGround::UENTTraceLocationToGround()
{
	NodeName = "TraceLocationToGround";
	ForceInstancing(true);
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;

	LocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTTraceLocationToGround, LocationKey));

	ObjectTypes.Add(ObjectTypeQuery1);
	ObjectTypes.Add(ObjectTypeQuery2);
}

void UENTTraceLocationToGround::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		LocationKey.ResolveSelectedKey(*BBAsset);
	}
}

void UENTTraceLocationToGround::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (bExecuteAtStart)
	{
		TraceToGround(OwnerComp);
	}
}

void UENTTraceLocationToGround::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	if (!bExecuteAtStart)
	{
		TraceToGround(OwnerComp);
	}
}

#if WITH_EDITOR
FString UENTTraceLocationToGround::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (LocationKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		KeyDesc = LocationKey.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("Trace %s to the ground"), *KeyDesc);

}
#endif

void UENTTraceLocationToGround::TraceToGround(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();

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

	FVector StartLocation = CurrentBlackboard->GetValue<UBlackboardKeyType_Vector>(LocationKey.GetSelectedKeyID());
	FVector EndLocation = StartLocation;
	EndLocation.Z -= GroundTraceLength;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Pawn);

	FHitResult HitResult;
	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(Pawn, StartLocation, EndLocation, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, false);

	if (bHit)
	{
		CurrentBlackboard->SetValue<UBlackboardKeyType_Vector>(LocationKey.GetSelectedKeyID(), HitResult.Location);
	}
}
