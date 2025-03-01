// Fill out your copyright notice in the Description page of Project Settings.


#include "Kevin/PlayerToNervePhysicConstraint.h"

#include "MainSettings.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kevin/Nerve.h"
#include "Kevin/UI/InGameUI.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/FirstPersonController.h"
#include "PROTOProfondeurs/Macro.h"


// Sets default values for this component's properties
UPlayerToNervePhysicConstraint::UPlayerToNervePhysicConstraint()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerToNervePhysicConstraint::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerToNervePhysicConstraint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float Distance = FVector::Dist(PlayerCharacter->GetActorLocation(), LinkedNerve->GetActorLocation());
	float Lerp = UKismetMathLibrary::NormalizeToRange(
			Distance,
			LinkedNerve->GetCableLength(),
			LinkedNerve->GetCableMaxExtension()
		);
	Lerp = FMath::Clamp(Lerp, 0.f, 1.f);
	Lerp = FMath::Sin((Lerp * PI) / 2.f);
	
	if (IsMovingTowardsPosition(LinkedNerve->GetActorLocation(), 0.2f))
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed * (1.f + Lerp);
	} else
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed * (1.f - Lerp);
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("%f"), Distance));
	if (Distance >= LinkedNerve->GetDistanceNeededToPropulsion() && !IsPropultionActive)
	{
		Cast<AFirstPersonController>(UGameplayStatics::GetPlayerController(this, 0))->GetCurrentInGameUI()->SetPropulsionActive(true);
		IsPropultionActive = true;
	} else if (Distance < LinkedNerve->GetDistanceNeededToPropulsion() && IsPropultionActive)
	{
		Cast<AFirstPersonController>(UGameplayStatics::GetPlayerController(this, 0))->GetCurrentInGameUI()->SetPropulsionActive(false);
		IsPropultionActive = false;
	}
}

void UPlayerToNervePhysicConstraint::Init(ANerve* vLinkedNerve, ACharacter* vPlayerCharacter)
{
	this->LinkedNerve = vLinkedNerve;
	this->PlayerCharacter = vPlayerCharacter;
	DefaultMaxSpeed = vPlayerCharacter->GetCharacterMovement()->MaxWalkSpeed;
}

void UPlayerToNervePhysicConstraint::ReleasePlayer()
{
	Cast<AFirstPersonController>(UGameplayStatics::GetPlayerController(this, 0))->GetCurrentInGameUI()->SetPropulsionActive(true);
	
	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed;
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
