// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTWeakZone.h"
#include "AkGameplayStatics.h"
#include "ENTElectricityComponent.h"
#include "Components/BoxComponent.h"
#include "Components/LightComponent.h"
#include "Components/PostProcessComponent.h"
#include "Config/ENTCoreConfig.h"
#include "Engine/Light.h"
#include "Player/ENTDefaultCharacter.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTWorldSave.h"
#include "Kismet/KismetMaterialLibrary.h"

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

	WeakZonePostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("WeakZonePostProcess"));
	WeakZonePostProcess->SetupAttachment(BoxComponent);
	WeakZonePostProcess->SetMobility(EComponentMobility::Static);
	WeakZonePostProcess->bUnbound = false;

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

	for (TObjectPtr<ALight> Light : CuredLights)
	{
		if (!Light || !Light->GetLightComponent())
		{
			continue;
		}

		Light->GetLightComponent()->SetVisibility(true);
		Light->GetLightComponent()->SetIntensity(0.0f);
	}

	for (TObjectPtr<ALight> Light : CorruptedLights)
	{
		if (!Light || !Light->GetLightComponent())
		{
			continue;
		}

		Light->GetLightComponent()->SetVisibility(true);
	}
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
		BlackAndWhiteShader->Settings.WeightedBlendables.Array.Empty();
		BlackAndWhiteShader->Settings.AddBlendable(DynamicZoneMaterial, 1.0f);
	}

	ChangeZoneSize(ZoneSize);

	CuredLightsIntensity.Empty();
	for (TObjectPtr<ALight> Light : CuredLights)
	{
		if (!Light || !Light->GetLightComponent())
		{
			continue;
		}

		CuredLightsIntensity.Add(Light->GetLightComponent()->Intensity);

#if WITH_EDITORONLY_DATA
		Light->GetLightComponent()->SetVisibility(bShowCuredLights);
#endif
	}

	CorruptedLightsIntensity.Empty();
	for (TObjectPtr<ALight> Light : CorruptedLights)
	{
		if (!Light || !Light->GetLightComponent())
		{
			continue;
		}

		CorruptedLightsIntensity.Add(Light->GetLightComponent()->Intensity);

#if WITH_EDITORONLY_DATA
		Light->GetLightComponent()->SetVisibility(!bShowCuredLights);
#endif
	}
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
	if (!BoxComponent) return;
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
	//BoxComponent->DestroyComponent();
}

void AENTWeakZone::CreateZone()
{
	if (!BoxComponent) return;
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
			IENTWeakZoneInterface::Execute_OnEnterWeakZone(OverlappingActor, true);
		}
	}

	bIsZoneActive = true;
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

	int CuredLightIndex = 0;
	for (TObjectPtr<ALight> Light : CuredLights)
	{
		if (!Light || !Light->GetLightComponent())
		{
			continue;
		}

		if (CuredLightsIntensity.IsValidIndex(CuredLightIndex))
		{
			Light->GetLightComponent()->SetIntensity(FMath::Lerp(0.0f, CuredLightsIntensity[CuredLightIndex++], Alpha));
		}
	}

	int CorruptedLightIndex = 0;
	for (TObjectPtr<ALight> Light : CorruptedLights)
	{
		if (!Light || !Light->GetLightComponent())
		{
			continue;
		}

		if (CorruptedLightsIntensity.IsValidIndex(CorruptedLightIndex))
		{
			Light->GetLightComponent()->SetIntensity(FMath::Lerp(CorruptedLightsIntensity[CorruptedLightIndex++], 0.0f, Alpha));
		}
	}
}

void AENTWeakZone::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UENTWeakZoneInterface>())
	{
		IENTWeakZoneInterface::Execute_OnEnterWeakZone(OtherActor, bIsZoneActive);
	}
}

void AENTWeakZone::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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
		ElectricityComponent->SetElectricityColor(ElectricityCureColor);
		ElectricityComponent->PlayElectricityAnimation(StartCurePoint);
	}
	else
	{
		OnElectricityMovementFinished();
	}

	OnCure.Broadcast();
	DestroyZone();
}

void AENTWeakZone::CorruptZone(AActor* StartCorruptPoint)
{
	if (StartCorruptPoint != nullptr)
	{
		ElectricityComponent->SetElectricityColor(ElectricityCorruptColor);
		ElectricityComponent->PlayElectricityAnimation(StartCorruptPoint);
	}
	else
	{
		OnElectricityMovementFinished();
	}

	OnCorrupt.Broadcast();
	CreateZone();
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
	if (!bIsZoneActive)
		CureTimeline.Play();
	else
		CureTimeline.Reverse();
}
