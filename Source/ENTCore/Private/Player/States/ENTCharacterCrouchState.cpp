// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/ENTCharacterCrouchState.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"

UENTCharacterCrouchState::UENTCharacterCrouchState()
{
	StateID = EENTCharacterStateID::Crouch;
	MoveSpeed = 200.0f;
}

void UENTCharacterCrouchState::StateInit(UENTCharacterStateMachine* InStateMachine)
{
	Super::StateInit(InStateMachine);

	DefaultHalfHeight = Character->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	DefaultCameraLocation = Character->GetCamera()->GetRelativeLocation();

	UENTCharacterState* FoundState = StateMachine->FindState(EENTCharacterStateID::Walk);
	if (!FoundState)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("No walk state found"));

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	UENTCharacterMoveState* WalkState = Cast<UENTCharacterMoveState>(FoundState);
	if (!WalkState)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("WalkState should be %s instead of %s"), *UENTCharacterMoveState::StaticClass()->GetName(), *WalkState->GetClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	DefaultSpeed = WalkState->GetMoveSpeed();
}

void UENTCharacterCrouchState::StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);
}

void UENTCharacterCrouchState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	if (!GetInputs().bInputCrouch || IsFalling())
	{
		if (CanStandUp())
		{
			LerpCrouch(DeltaTime, true);

			if (CurrentDuration == 0.0f)
			{
				StateMachine->ChangeState(IsFalling() ? EENTCharacterStateID::Fall : EENTCharacterStateID::Idle);
			}
		}
	}
	else if (GetInputs().bInputCrouch)
	{
		LerpCrouch(DeltaTime, false);
	}
}

void UENTCharacterCrouchState::StateExit_Implementation(const EENTCharacterStateID& NextStateID)
{
	Super::StateExit_Implementation(NextStateID);
}

void UENTCharacterCrouchState::LerpCrouch(float DeltaTime, bool bStandUp)
{
	int Factor = bStandUp ? -1 : 1;
	CurrentDuration += DeltaTime * Factor;
	CurrentDuration = FMath::Clamp(CurrentDuration, 0.0f, Duration);

	float CrouchAlpha = UKismetMathLibrary::NormalizeToRange(CurrentDuration, 0.0f, Duration);
	float TargetHeight = FMath::Lerp(DefaultHalfHeight, CrouchHalfHeight, CrouchAlpha);
	float TargetSpeed = FMath::Lerp(DefaultSpeed, MoveSpeed, CrouchAlpha);

	FVector TargetRelativeLocation = DefaultCameraLocation;
	TargetRelativeLocation.Z = FMath::Lerp(DefaultCameraLocation.Z, CameraHeight, CrouchAlpha);

	Character->GetCapsuleComponent()->SetCapsuleHalfHeight(TargetHeight);
	Character->GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;

	Character->GetCamera()->SetRelativeLocation(TargetRelativeLocation);
}

bool UENTCharacterCrouchState::CanStandUp() const
{
	FVector TraceLocation = Character->GetBottomLocation();
	TraceLocation.Z += DefaultHalfHeight;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);

	EDrawDebugTrace::Type DrawDebugTrace = EDrawDebugTrace::None;

#if WITH_EDITOR
	if (bDebugState)
	{
		DrawDebugTrace = EDrawDebugTrace::ForDuration;
	}
#endif

	FHitResult HitResult;
	return !UKismetSystemLibrary::CapsuleTraceSingle(Character, TraceLocation, TraceLocation, Character->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultHalfHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, DrawDebugTrace, HitResult, false, FLinearColor::Red, FLinearColor::Green, 0.0f);
}
