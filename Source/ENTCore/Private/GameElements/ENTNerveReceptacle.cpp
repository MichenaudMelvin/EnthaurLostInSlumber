// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTNerveReceptacle.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "ENTCameraShakeComponent.h"
#include "ENTElectricityComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Interface/ENTActivation.h"
#include "GameElements/ENTNerve.h"
#include "Components/ENTPhysicConstraint.h"
#include "ENTElectricityFeedback.h"
#include "ENTInteractableComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ENTDefaultCharacter.h"

AENTNerveReceptacle::AENTNerveReceptacle()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);
	RootComp->SetMobility(EComponentMobility::Static);

	NerveReceptacle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Receptacle"));
	NerveReceptacle->SetupAttachment(RootComp);
	NerveReceptacle->SetMobility(EComponentMobility::Static);

#if WITH_EDITORONLY_DATA
	NerveEndEditorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EditorNerveEnd"));
	NerveEndEditorMesh->SetupAttachment(RootComponent);
	NerveEndEditorMesh->SetMobility(EComponentMobility::Static);
	NerveEndEditorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NerveEndEditorMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	NerveEndEditorMesh->SetVisibility(false, true);
	NerveEndEditorMesh->bHiddenInGame = true;
	NerveEndEditorMesh->bIsEditorOnly = true;
#endif

	InteractableComponent = CreateDefaultSubobject<UENTInteractableComponent>(TEXT("Interaction"));
	InteractableComponent->OnInteract.AddDynamic(this, &AENTNerveReceptacle::Interaction);

	NerveReceptaclesNoises = CreateDefaultSubobject<UAkComponent>(TEXT("NerveReceptaclesNoises"));
	NerveReceptaclesNoises->SetupAttachment(NerveReceptacle);

	ElectricityComponent = CreateDefaultSubobject<UENTElectricityComponent>(TEXT("Electricity Component"));
}

void AENTNerveReceptacle::BeginPlay()
{
	Super::BeginPlay();

	NerveEndTargetTransform *= GetActorTransform();

	//InteractableComponent->AddInteractable(NerveReceptacle);

	if (!ElectricityComponent)
	{
		return;
	}

	ElectricityComponent->OnElectricityAnimationStarted.AddDynamic(this, &AENTNerveReceptacle::OnElectricityAnimationStarted);
	ElectricityComponent->OnElectricityRadiusFinished.AddDynamic(this, &AENTNerveReceptacle::OnElectricityRadiusFinished);
	ElectricityComponent->OnElectricityMovementUpdated.AddDynamic(this, &AENTNerveReceptacle::OnElectricityMovementUpdated);
	ElectricityComponent->OnElectricityMovementFinished.AddDynamic(this, &AENTNerveReceptacle::OnElectricityMovementFinished);
	ElectricityComponent->OnElectricityOpacityFinished.AddDynamic(this, &AENTNerveReceptacle::OnElectricityOpacityFinished);

	AENTNerve::OnHoldStateUpdate.AddUObject(this, &AENTNerveReceptacle::OnHoldStateChanged);
}

void AENTNerveReceptacle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AENTNerveReceptacle::OnHoldStateChanged(bool bIsHolding)
{
	if (bIsHolding)
	{
		InteractableComponent->AddInteractable(NerveReceptacle);
	}else
	{
		InteractableComponent->RemoveInteractable(NerveReceptacle);
	}
}


#if WITH_EDITORONLY_DATA

void AENTNerveReceptacle::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!NerveEndEditorMesh)
	{
		return;
	}

	NerveEndTargetTransform = NerveEndEditorMesh->GetRelativeTransform();
}

#endif

void AENTNerveReceptacle::Interaction(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent)
{
	AENTDefaultCharacter* Character = Cast<AENTDefaultCharacter>(Pawn);
	if (!Character) return;

	UENTPhysicConstraint* Constraint = Character->GetComponentByClass<UENTPhysicConstraint>();
	if (Constraint)
	{
		AENTNerve* Nerve = Constraint->GetLinkedNerve();
		if (!Nerve) return;

		if (LinkedNerve != nullptr) return;

		LinkedNerve = Nerve;
		Nerve->SetCurrentReceptacle(this);
		Character->EmitNoise(NoiseRange);
		UAkGameplayStatics::PostEvent(GrowlNoise, nullptr, 0, FOnAkPostEventCallback());
		OnNerveConnect();
		Constraint->ReleasePlayer();
		ElectricityComponent->PlayElectricityAnimation(Nerve);
	}
	else
	{
		if (LinkedNerve == nullptr || ElectricityComponent->IsAnimRunning()) return;
		LinkedNerve->Interaction(Controller, Pawn, InteractionComponent);
	}
}

void AENTNerveReceptacle::TriggerLinkedObjects(AENTNerve* Nerve)
{
	IsConnected = !IsConnected;
	OnNerveDisconnect(IsConnected);

	TArray<AActor*> Actors;
	ObjectReactive.GetKeys(Actors);

	AENTDefaultCharacter* Player = Cast<AENTDefaultCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (Player)
	{
		Player->GetCameraShake()->MakeBigCameraShake();
	}

	for (auto Actor : Actors)
	{
		if (Actor->Implements<UENTActivation>())
		{
			if (!IsValid(Actor))
			{
				continue;
			}

			if (Actor->Implements<UENTActivation>())
			{
				if (ObjectReactive[Actor] == ENerveReactiveInteractionType::ForceDefaultState)
				{
					IENTActivation::Execute_SetLock(Actor, true);
				}
				else
				{
					IENTActivation::Execute_Trigger(Actor);
				}
			}
		}
	}
}

bool AENTNerveReceptacle::CanTheNerveBeTaken() const
{
	return !ElectricityComponent->IsAnimRunning();
}

void AENTNerveReceptacle::DisableReceptacle()
{
	NerveReceptaclesNoises->PostAkEvent(DisabledNoise);
	LinkedNerve = nullptr;
}

#pragma region Electricity

void AENTNerveReceptacle::OnElectricityAnimationStarted(AActor* LinkedActor)
{
	//LinkedNerve = Cast<AENTNerve>(LinkedActor);
}

void AENTNerveReceptacle::OnElectricityRadiusFinished()
{
	OnNerveAnimationFinished.Broadcast();
}

void AENTNerveReceptacle::OnElectricityMovementUpdated(float Alpha)
{
	FVector TargetLocation = LinkedNerve->GetCablePosition(Alpha);
	ElectricityComponent->GetElectricityFeedback()->SetActorLocation(TargetLocation);
}

void AENTNerveReceptacle::OnElectricityMovementFinished()
{
	if (NerveReceptaclesNoises)
	{
		NerveReceptaclesNoises->PostAkEvent(EnabledNoise);
	}

	TriggerLinkedObjects(LinkedNerve);
}

void AENTNerveReceptacle::OnElectricityOpacityFinished()
{
	//LinkedNerve = nullptr;
}

#pragma endregion
