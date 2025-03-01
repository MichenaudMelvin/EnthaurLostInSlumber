// Fill out your copyright notice in the Description page of Project Settings.


#include "Kevin/Nerve.h"

#include "MainSettings.h"
#include "Components/InteractableComponent.h"
#include "GameFramework/Character.h"
#include "Kevin/NerveReceptacle.h"
#include "Kevin/PlayerToNervePhysicConstraint.h"
#include "Kevin/UI/InGameUI.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonController.h"
#include "PROTOProfondeurs/Macro.h"


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
}

void ANerve::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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
	NerveBall->SetRelativeLocation(GetDefault<UMainSettings>()->PawnGrabObjectOffset);

	PhysicConstraint = Cast<UPlayerToNervePhysicConstraint>(
		Pawn->AddComponentByClass(UPlayerToNervePhysicConstraint::StaticClass(), false, FTransform::Identity, false)
	);
	PhysicConstraint->Init(this, Cast<ACharacter>(Pawn));
	InteractableComponent->OnInteract.Clear();
}
