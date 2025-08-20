// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTNerveReceptacle.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
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

	FOnTimelineFloat UpdateEvent;
	FOnTimelineEvent FinishedEvent;

	UpdateEvent.BindDynamic(this, &AENTNerveReceptacle::ElectricityRadiusUpdate);
	FinishedEvent.BindDynamic(this, &AENTNerveReceptacle::ElectricityRadiusFinished);

	ElectricityRadiusTimeline.AddInterpFloat(FirstElectricityRadiusCurve, UpdateEvent);
	ElectricityRadiusTimeline.SetTimelineFinishedFunc(FinishedEvent);

	UpdateEvent.Unbind();
	FinishedEvent.Unbind();

	UpdateEvent.BindDynamic(this, &AENTNerveReceptacle::ElectricityMovementUpdate);
	FinishedEvent.BindDynamic(this, &AENTNerveReceptacle::ElectricityMovementFinished);

	ElectricityMovementTimeline.SetPlayRate(1 / ElectricityMovementDuration);
	ElectricityMovementTimeline.AddInterpFloat(ElectricityMovementCurve, UpdateEvent);
	ElectricityMovementTimeline.SetTimelineFinishedFunc(FinishedEvent);

	UpdateEvent.Unbind();
	FinishedEvent.Unbind();

	UpdateEvent.BindDynamic(this, &AENTNerveReceptacle::ElectricityOpacityUpdate);
	FinishedEvent.BindDynamic(this, &AENTNerveReceptacle::ElectricityOpacityFinished);

	ElectricityOpacityTimeline.SetPlayRate(1 / ElectricityOpacityDuration);
	ElectricityOpacityTimeline.AddInterpFloat(ElectricityOpacityCurve, UpdateEvent);
	ElectricityOpacityTimeline.SetTimelineFinishedFunc(FinishedEvent);
}

void AENTNerveReceptacle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ElectricityRadiusTimeline.TickTimeline(DeltaSeconds);
	ElectricityMovementTimeline.TickTimeline(DeltaSeconds);
	ElectricityOpacityTimeline.TickTimeline(DeltaSeconds);
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

void AENTNerveReceptacle::TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

#pragma region Timelines

void AENTNerveReceptacle::ElectricityRadiusUpdate(float Alpha)
{
	if (!NerveElectricityFeedback)
	{
		return;
	}

	float Radius = FMath::Lerp(StartRadiusTarget, EndRadiusTarget, Alpha);
	NerveElectricityFeedback->SetRadius(Radius);
}

void AENTNerveReceptacle::ElectricityRadiusFinished()
{
	if (EndRadiusTarget != SecondRadiusTarget)
	{
		return;
	}

	OnNerveAnimationFinished.Broadcast();
}

void AENTNerveReceptacle::ElectricityMovementUpdate(float Alpha)
{
	if (!NerveElectricityFeedback)
	{
		return;
	}

	FVector TargetLocation = LinkedNerve->GetCablePosition(Alpha);
	NerveElectricityFeedback->SetActorLocation(TargetLocation);
}

void AENTNerveReceptacle::ElectricityMovementFinished()
{
	if (NerveReceptaclesNoises)
	{
		NerveReceptaclesNoises->PostAkEvent(EnabledNoise);
	}

	TriggerLinkedObjects(LinkedNerve);

	StartRadiusTarget = FirstRadiusTarget;
	EndRadiusTarget = SecondRadiusTarget;
	ElectricityRadiusTimeline.SetFloatCurve(SecondElectricityRadiusCurve, FirstElectricityRadiusCurve.GetFName());
	ElectricityRadiusTimeline.SetPlayRate(1 / SecondElectricityRadiusDuration);
	ElectricityRadiusTimeline.PlayFromStart();

	ElectricityOpacityTimeline.PlayFromStart();
}

void AENTNerveReceptacle::ElectricityOpacityUpdate(float Alpha)
{
	if (!NerveElectricityFeedback)
	{
		return;
	}

	float ScalarParam = FMath::Lerp(1.0f, 0.0f, Alpha);
	NerveElectricityFeedback->GetMaterial()->SetScalarParameterValue(ElectricityOpacityParam, ScalarParam);
}

void AENTNerveReceptacle::ElectricityOpacityFinished()
{
	if (!NerveElectricityFeedback)
	{
		return;
	}

	NerveElectricityFeedback->Destroy();
	NerveElectricityFeedback = nullptr;
	LinkedNerve = nullptr;
}

#pragma endregion

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
	LinkedNerve = Nerve;

	if (!NerveElectricityFeedback)
	{
		return;
	}

	StartRadiusTarget = 0.0f;
	EndRadiusTarget = FirstRadiusTarget;
	ElectricityRadiusTimeline.SetFloatCurve(FirstElectricityRadiusCurve, SecondElectricityRadiusCurve.GetFName());
	ElectricityRadiusTimeline.SetPlayRate(1 / FirstElectricityRadiusDuration);
	ElectricityRadiusTimeline.PlayFromStart();

	ElectricityMovementTimeline.PlayFromStart();
}

