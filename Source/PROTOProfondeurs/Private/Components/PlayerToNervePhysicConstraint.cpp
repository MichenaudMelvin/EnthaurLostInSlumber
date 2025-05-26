// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerToNervePhysicConstraint.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameElements/Nerve.h"
#include "UI/InGameUI.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Physics/NervePhysicsConstraint.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"


UPlayerToNervePhysicConstraint::UPlayerToNervePhysicConstraint()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerToNervePhysicConstraint::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<AFirstPersonController>(UGameplayStatics::GetPlayerController(this, 0));
}

void UPlayerToNervePhysicConstraint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PlayerController->GetPlayerInputs().bInputInteractTrigger)
	{
		bHasReleasedInteraction = true;
	}

	float Distance = LinkedNerve->GetCableLength();
	float Lerp = UKismetMathLibrary::NormalizeToRange(
			Distance,
			0.0f,
			LinkedNerve->GetCableMaxExtension()
		);

	Lerp = FMath::Clamp(Lerp, 0.f, 1.f);
	Lerp = FMath::Sin((Lerp * PI) / 2.f);

	if (IsMovingTowardsPosition(LinkedNerve->GetLastCableLocation(), 0.2f))
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed * (1.f + Lerp);
	} else
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed * (1.f - Lerp);
	}

	if (!bHasReleasedInteraction)
	{
		return;
	}

	if (Distance >= LinkedNerve->GetDistanceNeededToPropulsion())
	{
		if (!IsPropultionActive)
		{
			PlayerController->GetCurrentInGameUI()->SetPropulsionActive(true);
			IsPropultionActive = true;
		}

		if (PlayerController->GetPlayerInputs().bInputInteractPressed && !IsAlreadyPropulsing)
		{
			IsAlreadyPropulsing = true;

			FVector CableDirection = LinkedNerve->GetCableDirection();
			const FVector CameraDirection = PlayerCharacter->GetCamera()->GetForwardVector();

			const FFloatRange& CameraRange = GetDefault<UNervePhysicsConstraint>()->CameraPropulsion;
			CableDirection.Z = FMath::Clamp(CameraDirection.Z, CameraRange.GetLowerBoundValue(), CameraRange.GetUpperBoundValue());

			const float Force = FMath::Lerp(LinkedNerve->GetPropulsionForceRange().GetLowerBoundValue(), LinkedNerve->GetPropulsionForceRange().GetUpperBoundValue(), Lerp);

			PlayerCharacter->EjectCharacter(CableDirection * Force);

			ReleasePlayer(true);
		}
	} else if (Distance < LinkedNerve->GetDistanceNeededToPropulsion())
	{

		if (IsPropultionActive)
		{
			PlayerController->GetCurrentInGameUI()->SetPropulsionActive(false);
			IsPropultionActive = false;
		}

		else if (PlayerController->GetPlayerInputs().bInputInteractPressed)
		{
			ReleasePlayer(true);
		}
	}
}

void UPlayerToNervePhysicConstraint::Init(ANerve* vLinkedNerve, ACharacter* vPlayerCharacter)
{
	LinkedNerve = vLinkedNerve;
	AFirstPersonCharacter* CastCharacter = Cast<AFirstPersonCharacter>(vPlayerCharacter);
	if (!CastCharacter)
	{
		return;
	}

	PlayerCharacter = CastCharacter;
	DefaultMaxSpeed = vPlayerCharacter->GetCharacterMovement()->MaxWalkSpeed;
}

void UPlayerToNervePhysicConstraint::ReleasePlayer(const bool DetachFromPlayer)
{
	if (PlayerCharacter && PlayerController)
	{
		PlayerController->GetCurrentInGameUI()->SetPropulsionActive(false);
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed;
	}

	if (DetachFromPlayer && LinkedNerve)
	{
		LinkedNerve->DetachNerveBall();
	}

	DestroyComponent();
}

bool UPlayerToNervePhysicConstraint::IsMovingTowardsPosition(const FVector& TargetPosition, float AcceptanceThreshold) const
{
	const UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement();
	if (!MovementComponent) return false;

	const FVector Velocity = MovementComponent->Velocity;
	if (Velocity.IsNearlyZero()) return false;

	const FVector CurrentLocation = PlayerCharacter->GetActorLocation();
	const FVector ToTargetDirection = (TargetPosition - CurrentLocation).GetSafeNormal();
	const FVector MovementDirection = Velocity.GetSafeNormal();

	const float DotProduct = FVector::DotProduct(MovementDirection, ToTargetDirection);
	return DotProduct >= AcceptanceThreshold;
}
