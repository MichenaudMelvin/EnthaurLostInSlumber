// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTWeakZone.h"
#include "AkGameplayStatics.h"
#include "ENTElectricityComponent.h"
#include "Components/BoxComponent.h"
#include "ENTInteractableComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameElements/ENTAmberOre.h"
#include "Kismet/KismetMathLibrary.h"
#include "Config/ENTCoreConfig.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/States/ENTCharacterStateMachine.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTWorldSave.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#if WITH_EDITORONLY_DATA
#include "Components/BillboardComponent.h"
#endif

AENTWeakZone::AENTWeakZone()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("WeakZone"));
	BoxComponent->SetupAttachment(RootComponent);

	BlackAndWhiteShader = CreateDefaultSubobject<UPostProcessComponent>(TEXT("Black and White Shader"));
	BlackAndWhiteShader->SetupAttachment(RootComponent);

#if WITH_EDITORONLY_DATA
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	BillboardComponent->SetupAttachment(RootComponent);
#endif

	ElectricityComponent = CreateDefaultSubobject<UENTElectricityComponent>(TEXT("Electricity Component"));
}

void AENTWeakZone::BeginPlay()
{
	Super::BeginPlay();

	const UENTCoreConfig* CoreConfig = GetDefault<UENTCoreConfig>();
	if (!CoreConfig)
	{
		return;
	}

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AENTWeakZone::OnZoneBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AENTWeakZone::OnZoneEndOverlap);

	// short delay because GetOverlappingActors does not work properly at the BeginPlay
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AENTWeakZone::InitZone, 0.2f, false);

	FOnTimelineFloat UpdateEvent;

	UpdateEvent.BindDynamic(this, &AENTWeakZone::CureUpdate);
	CureTimeline.AddInterpFloat(CureCurve, UpdateEvent);
	CureTimeline.SetPlayRate(1 / CureDuration);

	UpdateEvent.Unbind();

	ElectricityComponent->OnElectricityMovementFinished.AddDynamic(this, &AENTWeakZone::OnElectricityMovementFinished);
}

void AENTWeakZone::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!DynamicZoneMaterial)
	{
		DynamicZoneMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, ZoneMaterial);
	}

	if (DynamicZoneMaterial)
	{
		BlackAndWhiteShader->Settings.RemoveBlendable(DynamicZoneMaterial);
		BlackAndWhiteShader->Settings.AddBlendable(DynamicZoneMaterial, 1.0f);
	}

	ChangeZoneSize(ZoneSize);
}

void AENTWeakZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CureTimeline.TickTimeline(DeltaSeconds);
}

void AENTWeakZone::InitZone()
{
	BoxComponent->UpdateOverlaps();

	TArray<AActor*> OverlappingActors;
	BoxComponent->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor == nullptr)
		{
			continue;
		}

		if (OverlappingActor->Implements<UENTWeakZoneInterface>())
		{
			IENTWeakZoneInterface::Execute_OnEnterWeakZone(OverlappingActor, bIsZoneActive);
		}
	}
}

void AENTWeakZone::DestroyZone()
{
	TArray<AActor*> OverlappingActors;
	BoxComponent->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor == nullptr)
		{
			continue;
		}

		if (OverlappingActor->Implements<UENTWeakZoneInterface>())
		{
			IENTWeakZoneInterface::Execute_OnExitWeakZone(OverlappingActor);
		}
	}

	bIsZoneActive = false;
	BoxComponent->DestroyComponent();
}

void AENTWeakZone::ChangeZoneSize(const FVector& NewSize)
{
	BoxComponent->SetBoxExtent(NewSize);

	if (DynamicZoneMaterial)
	{
		float Radius = FMath::Sqrt(FMath::Pow(NewSize.X, 2) + FMath::Pow(NewSize.Y, 2));
		DynamicZoneMaterial->SetScalarParameterValue(RadiusParamName, Radius);
		DynamicZoneMaterial->SetVectorParameterValue(LocationParamName, GetActorLocation());
	}
}

void AENTWeakZone::CureUpdate(float Alpha)
{
	float ScalarParam = FMath::Lerp(1.0f, 0.0f, Alpha);
	DynamicZoneMaterial->SetScalarParameterValue(CureParam, ScalarParam);
}

void AENTWeakZone::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UENTWeakZoneInterface>())
	{
		IENTWeakZoneInterface::Execute_OnEnterWeakZone(OtherActor, bIsZoneActive);
	}
}

void AENTWeakZone::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->Implements<UENTWeakZoneInterface>())
	{
		IENTWeakZoneInterface::Execute_OnExitWeakZone(OtherActor);
	}
}

void AENTWeakZone::CureZone(AActor* StartCurePoint)
{
	if (StartCurePoint != nullptr)
	{
		ElectricityComponent->PlayElectricityAnimation(StartCurePoint);
	}
	else
	{
		OnElectricityMovementFinished();
	}
	DestroyZone();
}

void AENTWeakZone::ActivateZone(bool bActivateZone)
{
	if (bActivateZone)
	{
		ChangeZoneSize(ZoneSize);
	}
	else
	{
		ChangeZoneSize(FVector::ZeroVector);
	}
}

FENTGameElementData& AENTWeakZone::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	FENTWeakZoneData Data = FENTWeakZoneData();
	Data.bIsActive = bIsZoneActive;
	return CurrentWorldSave->WeakZoneData.Add(GetName(), Data);
}

void AENTWeakZone::LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave)
{
	const FENTWeakZoneData& Data = static_cast<const FENTWeakZoneData&>(GameElementData);
	if (!Data.bIsActive) CureZone(nullptr);
}

void AENTWeakZone::OnElectricityMovementFinished()
{
	CureTimeline.Play();
}
