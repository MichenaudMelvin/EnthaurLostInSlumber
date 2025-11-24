// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/ENTCopyBlackboardValue.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Rotator.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UENTCopyBlackboardValue::UENTCopyBlackboardValue()
{
	NodeName = "CopyBlackboardValue";
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;

	ForceInstancing(true);

	SourceBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UENTCopyBlackboardValue, SourceBlackboardKey));
	SourceBlackboardKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UENTCopyBlackboardValue, SourceBlackboardKey));
	SourceBlackboardKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UENTCopyBlackboardValue, SourceBlackboardKey));
	SourceBlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UENTCopyBlackboardValue, SourceBlackboardKey), UObject::StaticClass());
	SourceBlackboardKey.AddRotatorFilter(this, GET_MEMBER_NAME_CHECKED(UENTCopyBlackboardValue, SourceBlackboardKey));
	SourceBlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTCopyBlackboardValue, SourceBlackboardKey));

	DestinationBlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UENTCopyBlackboardValue, DestinationBlackboardKey));
	DestinationBlackboardKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UENTCopyBlackboardValue, DestinationBlackboardKey));
	DestinationBlackboardKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UENTCopyBlackboardValue, DestinationBlackboardKey));
	DestinationBlackboardKey.AddRotatorFilter(this, GET_MEMBER_NAME_CHECKED(UENTCopyBlackboardValue, DestinationBlackboardKey));
	DestinationBlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UENTCopyBlackboardValue, DestinationBlackboardKey));
}

void UENTCopyBlackboardValue::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		SourceBlackboardKey.ResolveSelectedKey(*BBAsset);
		DestinationBlackboardKey.ResolveSelectedKey(*BBAsset);
	}
}

void UENTCopyBlackboardValue::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (bExecuteAtStart)
	{
		CopyValue(OwnerComp);
	}
}

void UENTCopyBlackboardValue::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	if (!bExecuteAtStart)
	{
		CopyValue(OwnerComp);
	}
}

#if WITH_EDITOR
FString UENTCopyBlackboardValue::GetStaticDescription() const
{
	FString SourceKeyDesc = SourceBlackboardKey.SelectedKeyName.ToString();
	FString DestinationKeyDesc = DestinationBlackboardKey.SelectedKeyName.ToString();

	return FString::Printf(TEXT("Copy %s to %s"), *SourceKeyDesc, *DestinationKeyDesc);
}
#endif

void UENTCopyBlackboardValue::CopyValue(UBehaviorTreeComponent& OwnerComp) const
{
	UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();

	if (!CurrentBlackboard)
	{
		return;
	}

	if (SourceBlackboardKey.SelectedKeyType == UBlackboardKeyType_Bool::StaticClass())
	{
		bool boolValue = CurrentBlackboard->GetValue<UBlackboardKeyType_Bool>(SourceBlackboardKey.GetSelectedKeyID());

		if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Bool::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Bool>(DestinationBlackboardKey.GetSelectedKeyID(), boolValue);
		}
		else if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Float::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Float>(DestinationBlackboardKey.GetSelectedKeyID(), boolValue);
		}
		else if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Int::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Int>(DestinationBlackboardKey.GetSelectedKeyID(), boolValue);
		}
	}

	else if (SourceBlackboardKey.SelectedKeyType == UBlackboardKeyType_Float::StaticClass())
	{
		float floatValue = CurrentBlackboard->GetValue<UBlackboardKeyType_Float>(SourceBlackboardKey.GetSelectedKeyID());

		if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Bool::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Bool>(DestinationBlackboardKey.GetSelectedKeyID(), static_cast<bool>(floatValue));
		}
		else if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Float::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Float>(DestinationBlackboardKey.GetSelectedKeyID(), floatValue);
		}
		else if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Int::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Int>(DestinationBlackboardKey.GetSelectedKeyID(), floatValue);
		}
		else if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Rotator::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Rotator>(DestinationBlackboardKey.GetSelectedKeyID(), FRotator(floatValue));
		}
		else if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Vector>(DestinationBlackboardKey.GetSelectedKeyID(), FVector(floatValue));
		}
	}

	else if (SourceBlackboardKey.SelectedKeyType == UBlackboardKeyType_Int::StaticClass())
	{
		int32 intValue = CurrentBlackboard->GetValue<UBlackboardKeyType_Int>(SourceBlackboardKey.GetSelectedKeyID());

		if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Bool::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Bool>(DestinationBlackboardKey.GetSelectedKeyID(), static_cast<bool>(intValue));
		}
		else if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Float::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Float>(DestinationBlackboardKey.GetSelectedKeyID(), intValue);
		}
		else if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Int::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Int>(DestinationBlackboardKey.GetSelectedKeyID(), intValue);
		}
		else if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Rotator::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Rotator>(DestinationBlackboardKey.GetSelectedKeyID(), FRotator(intValue));
		}
		else if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Vector>(DestinationBlackboardKey.GetSelectedKeyID(), FVector(intValue));
		}
	}

	else if (SourceBlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* ObjectPtr = CurrentBlackboard->GetValue<UBlackboardKeyType_Object>(SourceBlackboardKey.GetSelectedKeyID());

		if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Bool::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Bool>(DestinationBlackboardKey.GetSelectedKeyID(), IsValid(ObjectPtr));
		}
		else
		{
			AActor* ActorPtr = Cast<AActor>(ObjectPtr);

			if (ActorPtr)
			{
				if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Rotator::StaticClass())
				{
					CurrentBlackboard->SetValue<UBlackboardKeyType_Rotator>(DestinationBlackboardKey.GetSelectedKeyID(), ActorPtr->GetActorRotation());
				}
				else if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
				{
					CurrentBlackboard->SetValue<UBlackboardKeyType_Vector>(DestinationBlackboardKey.GetSelectedKeyID(), ActorPtr->GetActorLocation());
				}
			}
		}
	}

	else if (SourceBlackboardKey.SelectedKeyType == UBlackboardKeyType_Rotator::StaticClass())
	{
		FRotator rotatorValue = CurrentBlackboard->GetValue<UBlackboardKeyType_Rotator>(SourceBlackboardKey.GetSelectedKeyID());

		if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Rotator::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Rotator>(DestinationBlackboardKey.GetSelectedKeyID(), rotatorValue);
		}
		else if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Vector>(DestinationBlackboardKey.GetSelectedKeyID(), rotatorValue.Vector());
		}
	}

	else if (SourceBlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		FVector vectorValue = CurrentBlackboard->GetValue<UBlackboardKeyType_Vector>(SourceBlackboardKey.GetSelectedKeyID());

		if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Rotator::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Rotator>(DestinationBlackboardKey.GetSelectedKeyID(), vectorValue.Rotation());
		}
		else if (DestinationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			CurrentBlackboard->SetValue<UBlackboardKeyType_Vector>(DestinationBlackboardKey.GetSelectedKeyID(), vectorValue);
		}
	}

#if WITH_EDITOR
	else
	{
		
	}
#endif
}
