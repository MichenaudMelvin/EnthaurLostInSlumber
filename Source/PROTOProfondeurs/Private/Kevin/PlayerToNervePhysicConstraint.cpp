// Fill out your copyright notice in the Description page of Project Settings.


#include "Kevin/PlayerToNervePhysicConstraint.h"

#include "MainSettings.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kevin/Nerve.h"
#include "Kismet/KismetMathLibrary.h"
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

	float Lerp = UKismetMathLibrary::NormalizeToRange(
		FVector::Dist(PlayerCharacter->GetActorLocation(), LinkedNerve->GetActorLocation()),
		LinkedNerve->GetCableLength(),
		LinkedNerve->GetCableMaxExtension()
	);
	Lerp = FMath::Clamp(Lerp, 0.f, 1.f);
	Lerp = FMath::Sin((Lerp * PI) / 2.f);

	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed * (1.f - Lerp);
}

void UPlayerToNervePhysicConstraint::Init(ANerve* vLinkedNerve, ACharacter* vPlayerCharacter)
{
	this->LinkedNerve = vLinkedNerve;
	this->PlayerCharacter = vPlayerCharacter;
	DefaultMaxSpeed = vPlayerCharacter->GetCharacterMovement()->MaxWalkSpeed;
}

void UPlayerToNervePhysicConstraint::ReleasePlayer()
{
	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed;
	DestroyComponent();
}
