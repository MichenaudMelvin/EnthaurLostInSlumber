// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTNerveReceptacle.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "FCTween.h"
#include "ENTCameraShakeComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Interface/ENTActivation.h"
#include "GameElements/ENTNerve.h"
#include "Components/ENTPropulsionConstraint.h"
#include "Gamefeel/ENTElectricityFeedback.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ENTDefaultCharacter.h"

AENTNerveReceptacle::AENTNerveReceptacle()
{
	PrimaryActorTick.bCanEverTick = false;

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

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetupAttachment(NerveReceptacle);
	Collision->SetMobility(EComponentMobility::Static);

	NerveReceptaclesNoises = CreateDefaultSubobject<UAkComponent>(TEXT("NerveReceptaclesNoises"));
	NerveReceptaclesNoises->SetupAttachment(NerveReceptacle);
}

void AENTNerveReceptacle::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AENTNerveReceptacle::TriggerEnter);

	NerveEndTargetTransform *= GetActorTransform();
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

void AENTNerveReceptacle::TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AENTNerve::StaticClass()))
	{
		AENTNerve* Nerve = Cast<AENTNerve>(OtherActor);
		Nerve->SetCurrentReceptacle(this);

		UAkGameplayStatics::PostEvent(GrowlNoise, nullptr, 0, FOnAkPostEventCallback());
		OnNerveConnect();

		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (Character)
		{
			UENTPropulsionConstraint* Constraint = Character->GetComponentByClass<UENTPropulsionConstraint>();

			if (Constraint)
			{
				Constraint->ReleasePlayer();
			}
		}

		PlayElectricityAnimation(Nerve);
	}
}

void AENTNerveReceptacle::TriggerLinkedObjects(AENTNerve* Nerve)
{
	IsConnected = !IsConnected;
	OnNerveDisconnect(IsConnected);

	TArray<AActor*> Actors;
	ObjectReactive.GetKeys(Actors);

	AENTDefaultCharacter* Player = Cast<AENTDefaultCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	Player->GetCameraShake()->MakeBigCameraShake();

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
				} else
				{
					IENTActivation::Execute_Trigger(Actor);
				}
			}
		}
	}
}

bool AENTNerveReceptacle::CanTheNerveBeTaken() const
{
	return !IsValid(NerveElectricityFeedback);
}

void AENTNerveReceptacle::DisableReceptacle()
{
	NerveReceptaclesNoises->PostAkEvent(DisabledNoise);
}

void AENTNerveReceptacle::PlayElectricityAnimation(AENTNerve* Nerve)
{
	AENTDefaultCharacter* Player = Cast<AENTDefaultCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	Player->GetCameraShake()->MakeSmallCameraShake();

	if (!ElectricityFeedbackClass)
	{
		return;
	}

	NerveElectricityFeedback = GetWorld()->SpawnActor<AENTElectricityFeedback>(ElectricityFeedbackClass, Nerve->GetActorTransform());
	KeepInMemoryNerve = Nerve;

	FCTween::Play(0.f, 30.f,
		[&](const float& F)
		{
			if (NerveElectricityFeedback)
			{
				NerveElectricityFeedback->SetRadius(F);
			}
		},
		.25f, EFCEase::InCubic);

	FCTween::Play(0.f, 1.f,
		[&](const float& F)
		{
			if (NerveElectricityFeedback)
			{
				FVector NewPos = KeepInMemoryNerve->GetCablePosition(F);
				NerveElectricityFeedback->SetActorLocation(NewPos);
			}
		},
		1.5f, EFCEase::Linear)->SetOnComplete([&]
		{
			NerveReceptaclesNoises->PostAkEvent(EnabledNoise);
			TriggerLinkedObjects(KeepInMemoryNerve);

			FCTween::Play(30.f, 200.f,
			[&](const float& F)
			{
				if (NerveElectricityFeedback)
				{
					NerveElectricityFeedback->SetRadius(F);
				}
			},
			1.f)->SetOnComplete([&]{OnNerveAnimationFinished.Broadcast();});

			FCTween::Play(1.f, 0.f,
			[&](const float& F)
			{
				if (NerveElectricityFeedback)
				{
					NerveElectricityFeedback->GetMaterial()->SetScalarParameterValue("Opacity", F);
				}
			},
			2.f)->SetOnComplete([&]
			{
				if (NerveElectricityFeedback)
				{
					NerveElectricityFeedback->Destroy();
					NerveElectricityFeedback = nullptr;
					KeepInMemoryNerve = nullptr;
				}
			});
		});
}

