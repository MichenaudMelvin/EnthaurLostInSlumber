// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ENTPhysicConstraint.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameElements/ENTNerve.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Config/ENTCoreConfig.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"
#include "Player/States/ENTCharacterMoveState.h"

UENTPhysicConstraint::UENTPhysicConstraint()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UENTPhysicConstraint::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<AENTDefaultPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
}

void UENTPhysicConstraint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UENTPhysicConstraint::UpdateDefaultMaxSpeed(UENTCharacterState* State, const EENTCharacterStateID& StateID)
{
	bShouldChildUpdateMaxSpeed = true;
	
	if (!State)
	{
		bShouldChildUpdateMaxSpeed = false;
		return;
	}

	UENTCharacterMoveState* MoveState = Cast<UENTCharacterMoveState>(State);
	if (!MoveState)
	{
		bShouldChildUpdateMaxSpeed = false;
		return;
	}

	DefaultMaxSpeed = MoveState->GetMoveSpeed();
}

void UENTPhysicConstraint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bShouldChildTick = true;

	if (!PlayerController->GetPlayerInputs().bInputInteractTrigger)
	{
		bHasReleasedInteraction = true;
	}

	if (!LinkedNerve)
	{
		bShouldChildTick = false;
		return;
	}

	if (!bHasReleasedInteraction)
	{
		bShouldChildTick = false;
	}
}

void UENTPhysicConstraint::Init(AENTNerve* vLinkedNerve, ACharacter* vPlayerCharacter)
{
	LinkedNerve = vLinkedNerve;
	AENTDefaultCharacter* CastCharacter = Cast<AENTDefaultCharacter>(vPlayerCharacter);
	if (!CastCharacter)
	{
		bShouldChildInit = false;
		return;
	}

	PlayerCharacter = CastCharacter;
	PlayerStateMachine = PlayerCharacter->GetStateMachine();
	if (!PlayerStateMachine)
	{
		bShouldChildInit = false;
	}
}

void UENTPhysicConstraint::ReleasePlayer(const bool DetachFromPlayer)
{
	if (PlayerCharacter && PlayerController)
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed;
	}

	if (DetachFromPlayer && LinkedNerve)
	{
		LinkedNerve->DetachNerveBall(false);
	}
	
	DestroyComponent();
}
