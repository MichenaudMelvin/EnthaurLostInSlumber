// Fill out your copyright notice in the Description page of Project Settings.


#include "Path/ENTJumpLink.h"

#include "AIController.h"
#include "NavLinkCustomComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Navigation/PathFollowingComponent.h"


AENTJumpLink::AENTJumpLink()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

#if WITH_EDITORONLY_DATA
	FistNavLinkDebugArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FistNavLinkDebugArrow"));
	FistNavLinkDebugArrow->SetupAttachment(Root);
	FistNavLinkDebugArrow->SetWorldRotation(FRotator(90.0f, 0.0f, 0.0f));
	FistNavLinkDebugArrow->SetArrowColor(FLinearColor::White);

	SecondNavLinkDebugArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("SecondNavLinkDebugArrow"));
	SecondNavLinkDebugArrow->SetupAttachment(Root);
	SecondNavLinkDebugArrow->SetWorldRotation(FRotator(90.0f, 0.0f, 0.0f));
	SecondNavLinkDebugArrow->SetArrowColor(FLinearColor::White);
#endif

	NavLink = CreateDefaultSubobject<UNavLinkCustomComponent>(TEXT("NavLink"));
	NavLink->SetMoveReachedLink(this, &AENTJumpLink::NotifyLinkReached);
	NavLink->SetNavigationRelevancy(true);
}

void AENTJumpLink::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetActorRotation(FRotator::ZeroRotator);
	SetActorScale3D(FVector::OneVector);
	NavLink->SetLinkData(FirstNavLinkLocation, SecondNavLinkLocation, ENavLinkDirection::BothWays);

#if WITH_EDITORONLY_DATA
	FistNavLinkDebugArrow->SetRelativeLocation(FirstNavLinkLocation);
	SecondNavLinkDebugArrow->SetRelativeLocation(SecondNavLinkLocation);
#endif
}

#if WITH_EDITOR
void AENTJumpLink::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.GetPropertyName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AENTJumpLink, FirstNavLinkLocation) || PropertyName == GET_MEMBER_NAME_CHECKED(AENTJumpLink, bIgnoreGroundTrace))
	{
		if (!bIgnoreGroundTrace)
		{
			FVector StartLocation = FirstNavLinkLocation + GetActorLocation();
			StartLocation.Z += GroundTraceLength;
			FVector EndLocation = FirstNavLinkLocation + GetActorLocation();
			EndLocation.Z -= GroundTraceLength;

			TArray<AActor*> ActorToIgnore;
			FHitResult HitResult;

			bool bHit = UKismetSystemLibrary::LineTraceSingle(this, StartLocation, EndLocation, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorToIgnore, EDrawDebugTrace::None, HitResult, true);

			if (bHit)
			{
				FirstNavLinkLocation = HitResult.Location - GetActorLocation();
			}
		}
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AENTJumpLink, SecondNavLinkLocation) || PropertyName == GET_MEMBER_NAME_CHECKED(AENTJumpLink, bIgnoreGroundTrace))
	{
		if (!bIgnoreGroundTrace)
		{
			FVector StartLocation = SecondNavLinkLocation + GetActorLocation();
			StartLocation.Z += GroundTraceLength;
			FVector EndLocation = SecondNavLinkLocation + GetActorLocation();
			EndLocation.Z -= GroundTraceLength;

			TArray<AActor*> ActorToIgnore;
			FHitResult HitResult;

			bool bHit = UKismetSystemLibrary::LineTraceSingle(this, StartLocation, EndLocation, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorToIgnore, EDrawDebugTrace::None, HitResult, true);

			if (bHit)
			{
				SecondNavLinkLocation = HitResult.Location - GetActorLocation();
			}
		}
	}
}
#endif

void AENTJumpLink::NotifyLinkReached(UNavLinkCustomComponent* NavLinkCustomComponent, UObject* PathingAgent, const UE::Math::TVector<double>& Destination)
{
	if (!NavLinkCustomComponent)
	{
		return;
	}

	UPathFollowingComponent* PathComp = Cast<UPathFollowingComponent>(PathingAgent);
	if (!PathComp)
	{
		return;
	}

	AActor* PathOwner = PathComp->GetOwner();
	if (!PathOwner)
	{
		return;
	}

	AAIController* ControllerOwner = Cast<AAIController>(PathOwner);
	if (!ControllerOwner)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = ControllerOwner->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	BlackboardComp->SetValueAsVector(JumpLocationKeyName, Destination);
	BlackboardComp->SetValueAsBool(JumpKeyName, true);
}
