// Fill out your copyright notice in the Description page of Project Settings.


#include "Kevin/Nerve.h"

#include "FCTween.h"
#include "Components/InteractableComponent.h"
#include "GameFramework/Character.h"
#include "Kevin/NerveReceptacle.h"
#include "Kevin/PlayerToNervePhysicConstraint.h"
#include "Player/CharacterSettings.h"

// Sets default values
ANerve::ANerve()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	CableComponent->SetupAttachment(RootComponent);

	NerveBall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	NerveBall->SetupAttachment(RootComponent);

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interaction"));
	InteractableComponent->OnInteract.AddDynamic(this, &ANerve::Interaction);
}

// Called when the game starts or when spawned
void ANerve::BeginPlay()
{
	Super::BeginPlay();

	InteractableComponent->AddInteractable(NerveBall);
	DefaultPosition = NerveBall->GetComponentLocation();
}

void ANerve::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ANerve::DetachNerveBall()
{
	NerveBall->SetSimulatePhysics(true);
	NerveBall->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	
	FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	NerveBall->AttachToComponent(GetRootComponent(), Rules);

	FCTween::Play(
			NerveBall->GetComponentLocation(),
			DefaultPosition,
			[&](const FVector& Pos)
			{
				NerveBall->SetWorldLocation(Pos);
			},
			1.f,
			EFCEase::OutElastic);

	if (!InteractableComponent->OnInteract.IsAlreadyBound(this, &ANerve::Interaction))
	{
		InteractableComponent->OnInteract.AddDynamic(this, &ANerve::Interaction);
	}
}

void ANerve::Interaction(APlayerController* PlayerController, APawn* Pawn)
{
	if (CurrentAttachedReceptacle != nullptr)
	{
		CurrentAttachedReceptacle->TriggerLinkedObjects();
		CurrentAttachedReceptacle = nullptr;
	}
	
	NerveBall->SetSimulatePhysics(false);
	NerveBall->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	NerveBall->AttachToComponent(Pawn->GetRootComponent(), Rules);
	NerveBall->SetRelativeLocation(GetDefault<UCharacterSettings>()->PawnGrabObjectOffset);

	PhysicConstraint = Cast<UPlayerToNervePhysicConstraint>(
		Pawn->AddComponentByClass(UPlayerToNervePhysicConstraint::StaticClass(), false, FTransform::Identity, false)
	);
	PhysicConstraint->Init(this, Cast<ACharacter>(Pawn));
	InteractableComponent->RemoveInteractable(NerveBall);
}
