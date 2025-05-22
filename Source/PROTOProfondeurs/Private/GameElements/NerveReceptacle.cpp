// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/NerveReceptacle.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "FCTween.h"
#include "Components/CameraShakeComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Interface/NerveReactive.h"
#include "GameElements/Nerve.h"
#include "Components/PlayerToNervePhysicConstraint.h"
#include "Gamefeel/ElectricityFeedback.h"
#include "Kismet/GameplayStatics.h"
#include "Parameters/BPRefParameters.h"
#include "Player/FirstPersonCharacter.h"

ANerveReceptacle::ANerveReceptacle()
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

void ANerveReceptacle::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ANerveReceptacle::TriggerEnter);

	NerveEndTargetTransform *= GetActorTransform();
}

#if WITH_EDITORONLY_DATA
void ANerveReceptacle::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	NerveEndTargetTransform = NerveEndEditorMesh->GetRelativeTransform();
}
#endif

void ANerveReceptacle::TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ANerve::StaticClass()))
	{
		ANerve* Nerve = Cast<ANerve>(OtherActor);
		Nerve->SetCurrentReceptacle(this);

		OnNerveConnect();

		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (Character)
		{
			UPlayerToNervePhysicConstraint* Constraint = Character->GetComponentByClass<UPlayerToNervePhysicConstraint>();

			if (Constraint)
			{
				Constraint->ReleasePlayer();
			}
		}

		NerveReceptaclesNoises->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
		PlayElectricityAnimation(Nerve);
		UAkGameplayStatics::PostEventAtLocation(ReceptacleEnabledNoise, NerveReceptacle->GetComponentLocation(), NerveReceptacle->GetComponentRotation(), this);
	}
}

void ANerveReceptacle::TriggerLinkedObjects(ANerve* Nerve)
{
	TArray<AActor*> Actors;
	ObjectReactive.GetKeys(Actors);

	AFirstPersonCharacter* Player = Cast<AFirstPersonCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	Player->GetCameraShake()->MakeBigCameraShake();

	for (auto Actor : Actors)
	{
		if (Actor->Implements<UNerveReactive>())
		{
			if (!IsValid(Actor))
			{
				continue;
			}

			if (Actor->Implements<UNerveReactive>())
			{
				if (ObjectReactive[Actor] == ENerveReactiveInteractionType::ForceDefaultState)
				{
					INerveReactive::Execute_SetLock(Actor, true);
				} else
				{
					INerveReactive::Execute_Trigger(Actor);
				}
			}
		}
	}
}

void ANerveReceptacle::PlayElectricityAnimation(ANerve* Nerve)
{
	AFirstPersonCharacter* Player = Cast<AFirstPersonCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	Player->GetCameraShake()->MakeSmallCameraShake();

	NerveElectricityFeedback = GetWorld()->SpawnActor<AElectricityFeedback>(GetDefault<UBPRefParameters>()->ElectricityFeedback, Nerve->GetActorTransform());
	KeepInMemoryNerve = Nerve;

	float Duration = KeepInMemoryNerve->GetCableLength() / 750.f;

	FCTween::Play(0.f, 30.f,
		[&](const float& F)
		{
			NerveElectricityFeedback->SetRadius(F);
		},
		.25f, EFCEase::InCubic);

	FCTween::Play(0.f, 1.f,
		[&](const float& F)
		{
			FVector NewPos = KeepInMemoryNerve->GetCablePosition(F);
			NerveElectricityFeedback->SetActorLocation(NewPos);
		},
		Duration, EFCEase::Linear)->SetOnComplete([&]
		{
			TriggerLinkedObjects(KeepInMemoryNerve);
			
			FCTween::Play(30.f, 200.f,
			[&](const float& F)
			{
				NerveElectricityFeedback->SetRadius(F);
			},
			1.f)->SetOnComplete([&]{OnNerveAnimationFinished.Broadcast();});

			FCTween::Play(1.f, 0.f,
			[&](const float& F)
			{
				NerveElectricityFeedback->GetMaterial()->SetScalarParameterValue("Opacity", F);
			},
			2.f)->SetOnComplete([&]
			{
				NerveElectricityFeedback->Destroy();
				NerveElectricityFeedback = nullptr;
				KeepInMemoryNerve = nullptr;
			});
		});
}

