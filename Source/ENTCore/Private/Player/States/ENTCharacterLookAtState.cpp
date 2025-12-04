// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/ENTCharacterLookAtState.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"

UENTCharacterLookAtState::UENTCharacterLookAtState()
{
	StateID = EENTCharacterStateID::LookAt;
	bAllowCameraMovement = false;
}

void UENTCharacterLookAtState::StateInit(UENTCharacterStateMachine* InStateMachine)
{
	Super::StateInit(InStateMachine);

	FOnTimelineFloat UpdateEvent;
	FOnTimelineEvent FinishedEvent;

	UpdateEvent.BindDynamic(this, &UENTCharacterLookAtState::LookAtUpdate);
	FinishedEvent.BindDynamic(this, &UENTCharacterLookAtState::FinishLookAt);

	LookAtTimeline.AddInterpFloat(StateCurve, UpdateEvent, "LookAtProperty", "LookAtTrack");
	LookAtTimeline.SetTimelineFinishedFunc(FinishedEvent);
}

void UENTCharacterLookAtState::StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	LookAtTimeline.SetPlayRate(1 / StateDuration);
	LookAtTimeline.PlayFromStart();
}

void UENTCharacterLookAtState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	LookAtTimeline.TickTimeline(DeltaTime);
}

void UENTCharacterLookAtState::LookAtUpdate(float Alpha)
{
	FQuat Result = FQuat::Slerp(StartRotation.Quaternion(), TargetRotation.Quaternion(), Alpha);
	Controller->SetControlRotation(Result.Rotator());
}

void UENTCharacterLookAtState::FinishLookAt()
{
	OnFinishLookAt.Broadcast();
	StateMachine->ChangeState(NextState);
}

void UENTCharacterLookAtState::SetLookAtLocation(const FVector& InLocationToLookAt, float InStateDuration, UCurveFloat* CurveFloat, EENTCharacterStateID InNextState)
{
	StateDuration = InStateDuration;

	StartRotation = Controller->GetControlRotation();
	TargetRotation = FRotationMatrix::MakeFromX(InLocationToLookAt - Character->GetActorLocation()).Rotator();

	NextState = InNextState; 

	if (CurveFloat)
	{
		LookAtTimeline.SetFloatCurve(CurveFloat, "LookAtTrack");
	}
}
