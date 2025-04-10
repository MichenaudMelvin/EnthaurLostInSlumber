// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/Nerve.h"
#include "FCTween.h"
#include "Components/InteractableComponent.h"
#include "GameFramework/Character.h"
#include "GameElements/NerveReceptacle.h"
#include "Components/PlayerToNervePhysicConstraint.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/CharacterSettings.h"
#include "Player/FirstPersonController.h"

ANerve::ANerve()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	UCableComponent* RootCable = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	RootCable->SetupAttachment(RootComponent);

	RootCable->AttachEndTo.ComponentProperty = GET_MEMBER_NAME_CHECKED(ANerve, NerveBall);

	Cables.Empty();

	InitCable(RootCable);
	Cables.Add(RootCable);

	NerveBall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	NerveBall->SetupAttachment(RootComponent);

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interaction"));
	InteractableComponent->OnInteract.AddDynamic(this, &ANerve::Interaction);

	CableColliders.Add(ObjectTypeQuery1);
	CableColliders.Add(ObjectTypeQuery2);
}

void ANerve::BeginPlay()
{
	Super::BeginPlay();

	InteractableComponent->AddInteractable(NerveBall);
	DefaultNervePosition = NerveBall->GetComponentLocation();
}

void ANerve::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	NerveBall->SetRelativeLocation(FVector(StartCableLength, 0.0f, 0.0f));

	if (StartCableLength > CableMaxExtension)
	{
		CableMaxExtension = StartCableLength + 1000.0f;
	}

	InitCable(Cables[0]);
}

void ANerve::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ApplyCablesPhysics();
}

#pragma region Cables

void ANerve::InitCable(const TObjectPtr<UCableComponent>& Cable) const
{
	if (!Cable)
	{
		return;
	}

	// Cable->bAttachEnd = false;
	Cable->AttachEndTo.ComponentProperty = GET_MEMBER_NAME_CHECKED(ANerve, NerveBall);
	Cable->EndLocation = FVector::ZeroVector;
	Cable->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Cable->SetCollisionResponseToAllChannels(ECR_Ignore);
	Cable->SetGenerateOverlapEvents(false);
	Cable->CableLength = 0.0f;
	Cable->SolverIterations = 16;
	Cable->SetMaterial(0, CableMaterial);
}

void ANerve::ApplyCablesPhysics()
{
	if (!bShouldApplyCablePhysics)
	{
		return;
	}

	int CurrentIndex = Cables.Num() - 1;

	UCableComponent* CurrentCable = Cables[CurrentIndex];

	if (!CurrentCable)
	{
		return;
	}

	FVector CurrentCableStartLocation = CurrentCable->GetComponentLocation();
	FVector CurrentCableEndLocation = CurrentCable->GetAttachedComponent()->GetComponentLocation();

	TArray<AActor*> Ignore;
	Ignore.Add(this);
	FHitResult Hit;

	bool bHit;

	if (Cables.Num() >= 2)
	{
		UCableComponent* LastCable = Cables[CurrentIndex - 1];
		FVector LastCableStartLocation = LastCable->GetComponentLocation();
		bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, LastCableStartLocation, CurrentCableEndLocation, CableColliders, false, Ignore, EDrawDebugTrace::None, Hit, true);

		if (!bHit)
		{
			LastCable->AttachEndTo.ComponentProperty = GET_MEMBER_NAME_CHECKED(ANerve, NerveBall);
			LastCable->EndLocation = FVector::ZeroVector;
			Cables.Remove(CurrentCable);
			CurrentCable->DestroyComponent(true);
			return;
		}
	}

	bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, CurrentCableStartLocation, CurrentCableEndLocation, CableColliders, false, Ignore, EDrawDebugTrace::None, Hit, true);

	if (!bHit)
	{
		return;
	}

	UActorComponent* Comp = AddComponentByClass(UCableComponent::StaticClass(), false, FTransform::Identity, false);
	if (!Comp)
	{
		return;
	}

	UCableComponent* CastComp = Cast<UCableComponent>(Comp);
	if (!CastComp)
	{
		return;
	}

	FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(CurrentCableEndLocation, Hit.Location);

	bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, CurrentCableEndLocation, Hit.Location, CableColliders, false, Ignore, EDrawDebugTrace::None, Hit, true);
	if (!bHit)
	{
		CastComp->DestroyComponent(true);
		return;
	}

	FVector WorldLocation = Hit.Location - (Direction * CableOffset);
	FVector RelativeLocation = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), WorldLocation);

	CastComp->SetWorldLocation(WorldLocation);

	CastComp->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
	InitCable(CastComp);

	CurrentCable->AttachEndTo.ComponentProperty = NAME_None;
	CurrentCable->EndLocation = RelativeLocation;

	Cables.Add(CastComp);
}

FVector ANerve::GetLastCableLocation() const
{
	int LastIndex = Cables.Num() - 1;

	if (!Cables[LastIndex])
	{
		return FVector::ZeroVector;
	}

	return Cables[LastIndex]->GetComponentLocation();
}

float ANerve::GetCableLength() const
{
	float Distance = 0.0f;
	for (TObjectPtr<UCableComponent> Cable : Cables)
	{
		FVector NerveBallRelativeLocation = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), NerveBall->GetComponentLocation());
		FVector EndLocation = Cable->AttachEndTo.ComponentProperty == NAME_None ? Cable->EndLocation : NerveBallRelativeLocation;

		Distance += FVector::Dist(Cable->GetRelativeLocation(), EndLocation);
	}

	return Distance;
}

FVector ANerve::GetCableDirection() const
{
	int LastIndex = Cables.Num() - 1;

	if (!Cables[LastIndex])
	{
		return FVector::ZeroVector;
	}

	return UKismetMathLibrary::GetDirectionUnitVector(Cables[LastIndex]->GetComponentLocation(), NerveBall->GetComponentLocation());
}

#pragma endregion

#pragma region NerveBall

void ANerve::AttachNerveBall(AActor* ActorToAttach)
{
	// NerveBall->SetSimulatePhysics(false);
	NerveBall->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	bShouldApplyCablePhysics = true;

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	NerveBall->AttachToComponent(ActorToAttach->GetRootComponent(), Rules);
	NerveBall->SetRelativeLocation(GetDefault<UCharacterSettings>()->PawnGrabObjectOffset);
}

void ANerve::DetachNerveBall()
{
	// NerveBall->SetSimulatePhysics(true);
	NerveBall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	PlayerController = nullptr;
	bShouldApplyCablePhysics = false;

	FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, true);

	NerveBall->AttachToComponent(RootComponent, Rules);

	FCTween::Play(
			NerveBall->GetComponentLocation(),
			DefaultNervePosition,
			[&](const FVector& Pos)
			{
				NerveBall->SetWorldLocation(Pos);
			},
			1.f,
			EFCEase::OutElastic);

	// delete all cables except the first one
	for (int i = Cables.Num() - 1; i > 0; i--)
	{
		UCableComponent* CurrentCable = Cables[i];
		Cables.RemoveAt(i);
		CurrentCable->DestroyComponent(true);
	}

	if(Cables[0])
	{
		Cables[0]->AttachEndTo.ComponentProperty = GET_MEMBER_NAME_CHECKED(ANerve, NerveBall);
		Cables[0]->EndLocation = FVector::ZeroVector;
	}

	InteractableComponent->AddInteractable(NerveBall);
	if (!InteractableComponent->OnInteract.IsAlreadyBound(this, &ANerve::Interaction))
	{
		InteractableComponent->OnInteract.AddDynamic(this, &ANerve::Interaction);
	}
}

bool ANerve::IsNerveBallAttached() const
{
	return !NerveBall->IsAttachedTo(RootComponent);
}

#pragma endregion

#pragma region Interaction

void ANerve::Interaction(APlayerController* InPlayerController, APawn* Pawn)
{
	if (CurrentAttachedReceptacle != nullptr)
	{
		CurrentAttachedReceptacle->TriggerLinkedObjects();
		CurrentAttachedReceptacle = nullptr;
	}

	PlayerController = Cast<AFirstPersonController>(InPlayerController);
	AttachNerveBall(Pawn);

	PhysicConstraint = Cast<UPlayerToNervePhysicConstraint>(
		Pawn->AddComponentByClass(UPlayerToNervePhysicConstraint::StaticClass(), false, FTransform::Identity, false)
	);

	PhysicConstraint->Init(this, Cast<ACharacter>(Pawn));
	InteractableComponent->RemoveInteractable(NerveBall);
}

#pragma endregion
