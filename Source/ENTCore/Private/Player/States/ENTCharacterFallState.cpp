// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/ENTCharacterFallState.h"

#include "AkGameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/ENTDefaultCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"

UENTCharacterFallState::UENTCharacterFallState()
{
	StateID = EENTCharacterStateID::Fall;

#if WITH_EDITORONLY_DATA
	DrawDebugNoiseDuration = 10.0f;
#endif
}

void UENTCharacterFallState::StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	if (PreviousStateID != EENTCharacterStateID::Jump && PreviousStateID != EENTCharacterStateID::Fall && PreviousStateID != EENTCharacterStateID::Stop)
	{
		bCanDoCoyoteTime = true;
		CoyoteTime = 0.0f;
	}
	else
	{
		bCanDoCoyoteTime = false;
	}

	bShouldEmitNoises = false;
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	Character->GetCharacterMovement()->GravityScale = GravityScale;
	Character->GetCharacterMovement()->AirControl = AirControl;
	Character->GetCharacterMovement()->BrakingDecelerationFalling = FallingDeceleration;
	LaunchCharacter();
}

void UENTCharacterFallState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	ManageWindSFX();

	if(bCanDoCoyoteTime)
	{
		CoyoteTime += DeltaTime;
		if(CoyoteTime >= CoyoteTimeMaxDuration)
		{
			bCanDoCoyoteTime = false;

#if WITH_EDITORONLY_DATA
			if(bDebugState)
			{
				FVector CapsuleLocation = Character->GetCapsuleComponent()->GetComponentLocation();
				FRotator CapsuleRotation = Character->GetCapsuleComponent()->GetComponentRotation();
				float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
				UKismetSystemLibrary::DrawDebugCapsule(Character, CapsuleLocation, CapsuleHalfHeight, CapsuleRadius, CapsuleRotation, FLinearColor::Red, 5.0f);
			}
#endif
		}

		if (GetInputs().bInputJump)
		{
			StateMachine->ChangeState(EENTCharacterStateID::Jump);
		}
	}

	if(Character->GetCharacterMovement()->IsFalling())
	{
		CharacterVelocity = FMath::Abs(Character->GetCharacterMovement()->Velocity.Z);
		return;
	}

	// emit noise on landing
	bShouldEmitNoises = true;
	EmitNoise();

	SetProjectionVelocity(FVector::ZeroVector, false);
	StateMachine->ChangeState(EENTCharacterStateID::Idle);
}

void UENTCharacterFallState::StateExit_Implementation(const EENTCharacterStateID& NextStateID)
{
	Super::StateExit_Implementation(NextStateID);

	SetProjectionVelocity(FVector::ZeroVector, false);

	if (bIsWindSFXPlaying)
	{
		UAkGameplayStatics::PostEvent(StopWindEvent, Character, 0, FOnAkPostEventCallback());
		bIsWindSFXPlaying = false;
	}
}

#if WITH_EDITORONLY_DATA
void UENTCharacterFallState::PostLoad()
{
	Super::PostLoad();

	DebugNoiseRange = VelocityNoiseFactor * CharacterVelocity;
}

void UENTCharacterFallState::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName& ChangedProperty = PropertyChangedEvent.GetMemberPropertyName();

	if (ChangedProperty == GET_MEMBER_NAME_CHECKED(UENTCharacterFallState, CharacterVelocity) || ChangedProperty == GET_MEMBER_NAME_CHECKED(UENTCharacterFallState, VelocityNoiseFactor) || ChangedProperty == GET_MEMBER_NAME_CHECKED(UENTCharacterFallState, NoiseRange))
	{
		DebugNoiseRange = VelocityNoiseFactor * CharacterVelocity;
	}
}

#endif

void UENTCharacterFallState::EmitNoise()
{
	if (!bDoesMakeNoise || !bShouldEmitNoises)
	{
		return;
	}

	NoiseRange = VelocityNoiseFactor * CharacterVelocity;

	Super::EmitNoise();

#if WITH_EDITORONLY_DATA
	if (bDebugState)
	{
		const FString Message = FString::Printf(TEXT("Emitted a noise of loudness %f and a range of %fcm, Character velocity was %fcm/s"), Loudness, NoiseRange, CharacterVelocity);

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, Message);
		FMessageLog("BlueprintLog").Message(EMessageSeverity::Info, FText::FromString(Message));
	}
#endif
}

void UENTCharacterFallState::ManageWindSFX()
{
	float VelocityLength = Character->GetVelocity().Length();

	UAkGameplayStatics::SetRTPCValue(SpeedVolume, VelocityLength, 0, Character);

	if (VelocityLength >= WindRequiredVelocity && !bIsWindSFXPlaying)
	{
		UAkGameplayStatics::PostEvent(StartWindEvent, Character, 0, FOnAkPostEventCallback());
		bIsWindSFXPlaying = true;
	}
	else if (VelocityLength < WindRequiredVelocity && bIsWindSFXPlaying)
	{
		UAkGameplayStatics::PostEvent(StopWindEvent, Character, 0, FOnAkPostEventCallback());
		bIsWindSFXPlaying = false;
	}
}

void UENTCharacterFallState::SetProjectionVelocity(const FVector& Velocity, bool bOverrideVelocity)
{
	ProjectionVelocity = Velocity;
	bOverrideCurrentVelocity = bOverrideVelocity;
}

void UENTCharacterFallState::LaunchCharacter() const
{
	FVector TargetVelocity = ProjectionVelocity;
	if (!bOverrideCurrentVelocity)
	{
		TargetVelocity += Character->GetCharacterMovement()->Velocity;
	}

	Character->GetCharacterMovement()->Velocity = TargetVelocity;
}
