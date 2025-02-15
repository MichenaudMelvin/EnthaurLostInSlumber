// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/FirstPersonCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/LocalPlayer.h"
#include "Player/States/CharacterState.h"
#include "Player/States/CharacterStateMachine.h"

AFirstPersonCharacter::AFirstPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);

	// Create a CameraComponent	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f)); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	CharacterMesh->SetOnlyOwnerSee(true);
	CharacterMesh->SetupAttachment(CameraComponent);
	CharacterMesh->bCastDynamicShadow = false;
	CharacterMesh->CastShadow = false;
	CharacterMesh->SetRelativeLocation(FVector(-30.0f, 0.0f, -150.0f));
}

void AFirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	CreateStates();
	InitStateMachine();
}

void AFirstPersonCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickStateMachine(DeltaSeconds);
}

void AFirstPersonCharacter::InitStateMachine()
{
	StateMachine = NewObject<UCharacterStateMachine>();

	if (StateMachine == nullptr)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Failed to create StateMachine"));

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
		return;
	}

	StateMachine->InitStateMachine(this);
}

void AFirstPersonCharacter::TickStateMachine(float DeltaTime)
{
	if (StateMachine == nullptr)
	{
		return;
	}

	StateMachine->TickStateMachine(DeltaTime);
}

void AFirstPersonCharacter::CreateStates()
{
	for (const TTuple<ECharacterStateID, TSubclassOf<UCharacterState>>& State : CharacterStates)
	{
		if (State.Key == ECharacterStateID::None)
		{
#if WITH_EDITOR
			const FString Message = FString::Printf(TEXT("Cannot create state because stateID is None"));

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
			FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
			continue;
		}

		if (State.Value == nullptr)
		{
#if WITH_EDITOR
			const FString Message = FString::Printf(TEXT("Cannot create state because stateClass is nullptr"));

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
			FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
			continue;
		}

		UCharacterState* NewState = NewObject<UCharacterState>(this, State.Value);
		if (NewState == nullptr)
		{
#if WITH_EDITOR
			const FString Message = FString::Printf(TEXT("Failed to create state %d"), State.Key);

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
			FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
			continue;
		}

		States.Add(NewState);
	}
}

#if WITH_EDITORONLY_DATA
void AFirstPersonCharacter::DisplayStates(bool bDisplay)
{
	bDebugStates = bDisplay;
}
#endif