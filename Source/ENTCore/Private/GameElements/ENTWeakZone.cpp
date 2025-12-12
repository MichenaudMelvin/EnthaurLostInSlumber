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
#include "Kismet/KismetMathLibrary.h"

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
	FOnTimelineEvent FinishEvent;

	UpdateEvent.BindDynamic(this, &AENTWeakZone::CureUpdate);
	FinishEvent.BindDynamic(this, &AENTWeakZone::CureFinish);
	CureTimeline.AddInterpFloat(CureCurve, UpdateEvent);
	CureTimeline.SetTimelineFinishedFunc(FinishEvent);
	CureTimeline.SetPlayRate(1 / CureDuration);

	UpdateEvent.Unbind();

	ElectricityComponent->OnElectricityMovementFinished.AddDynamic(this, &AENTWeakZone::OnElectricityMovementFinished);

	if (WeakZonePostProcess)
	{
		WeakZonePostProcess->bEnabled = true;;
	}

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

	SetActorsVisibility(false);
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

#if WITH_EDITORONLY_DATA
		float ScalarParam = bShowWeakZoneAsCure ? 0.0f : 1.0f;
		DynamicZoneMaterial->SetScalarParameterValue(CureParam, ScalarParam);
#endif
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
		Light->GetLightComponent()->SetVisibility(bShowWeakZoneAsCure);
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
		Light->GetLightComponent()->SetVisibility(!bShowWeakZoneAsCure);
#endif
	}

#if WITH_EDITORONLY_DATA
	WeakZonePostProcess->bEnabled = !bShowWeakZoneAsCure;
#endif
}

#if WITH_EDITOR
void AENTWeakZone::PostLoad()
{
	Super::PostLoad();

	SetActorsVisibility(false);
}

void AENTWeakZone::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	if (!PropertyAboutToChange)
	{
		return;
	}

	if (PropertyAboutToChange->NamePrivate == GET_MEMBER_NAME_CHECKED(AENTWeakZone, CuredActors))
	{
		SetCuredActorsVisibility(true);
	}
	else if (PropertyAboutToChange->NamePrivate == GET_MEMBER_NAME_CHECKED(AENTWeakZone, CorruptedActors))
	{
		SetCorruptedActorsVisibility(true);
	}
}

void AENTWeakZone::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName& ChangedProperty = PropertyChangedEvent.GetMemberPropertyName();

	if (ChangedProperty == GET_MEMBER_NAME_CHECKED(AENTWeakZone, CuredActors) || ChangedProperty == GET_MEMBER_NAME_CHECKED(AENTWeakZone, CorruptedActors) || ChangedProperty == GET_MEMBER_NAME_CHECKED(AENTWeakZone, bShowWeakZoneAsCure))
	{
		SetActorsVisibility(bShowWeakZoneAsCure);
	}
}
#endif

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
		if (!OverlappingActor)
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
	if (!BoxComponent)
	{
		return;
	}

	FVector LastBoxExtent = BoxComponent->GetUnscaledBoxExtent();
	BoxComponent->SetBoxExtent(ZoneSize, true);

	TArray<AActor*> OverlappingActors;
	BoxComponent->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (!OverlappingActor)
		{
			continue;
		}

		if (OverlappingActor->Implements<UENTWeakZoneInterface>())
		{
			IENTWeakZoneInterface::Execute_OnExitWeakZone(OverlappingActor);
		}
	}

	BoxComponent->SetBoxExtent(LastBoxExtent, false);

	bIsZoneActive = false;
}

void AENTWeakZone::CreateZone()
{
	if (!BoxComponent)
	{
		return;
	}

	FVector LastBoxExtent = BoxComponent->GetUnscaledBoxExtent();
	BoxComponent->SetBoxExtent(ZoneSize, true);

	TArray<AActor*> OverlappingActors;
	BoxComponent->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (!OverlappingActor)
		{
			continue;
		}

		if (OverlappingActor->Implements<UENTWeakZoneInterface>())
		{
			IENTWeakZoneInterface::Execute_OnEnterWeakZone(OverlappingActor, true);
		}
	}

	BoxComponent->SetBoxExtent(LastBoxExtent, false);

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

	FVector TargetZoneSize = UKismetMathLibrary::VLerp(ZoneSize, FVector::ZeroVector, Alpha);
	BoxComponent->SetBoxExtent(TargetZoneSize, false);

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

void AENTWeakZone::CureFinish()
{
	FVector CurrentBoxExtent = BoxComponent->GetUnscaledBoxExtent();
	BoxComponent->SetBoxExtent(CurrentBoxExtent, true);
}

void AENTWeakZone::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CureTimeline.IsPlaying() || CureTimeline.IsReversing())
	{
		return;
	}

	if (OtherActor->Implements<UENTWeakZoneInterface>())
	{
		IENTWeakZoneInterface::Execute_OnEnterWeakZone(OtherActor, bIsZoneActive);
	}
}

void AENTWeakZone::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CureTimeline.IsPlaying() || CureTimeline.IsReversing())
	{
		return;
	}

	if (OtherActor->Implements<UENTWeakZoneInterface>())
	{
		IENTWeakZoneInterface::Execute_OnExitWeakZone(OtherActor);
	}
}

void AENTWeakZone::SetActorsVisibility(bool bCure) const
{
	SetCuredActorsVisibility(bCure);
	SetCorruptedActorsVisibility(!bCure);
}

void AENTWeakZone::SetCuredActorsVisibility(bool bVisible) const
{
	SetArrayVisibility(bVisible, CuredActors);
}

void AENTWeakZone::SetCorruptedActorsVisibility(bool bVisible) const
{
	SetArrayVisibility(bVisible, CorruptedActors);
}

void AENTWeakZone::SetArrayVisibility(bool bVisible, const TArray<TObjectPtr<AActor>>& ActorArray) const
{
	for (TObjectPtr<AActor> Actor : ActorArray)
	{
		if (Actor && Actor->GetRootComponent())
		{
			USceneComponent* ActorRootComp = Actor->GetRootComponent();
			ActorRootComp->SetVisibility(bVisible, true);

			APostProcessVolume* PostProcess = Cast<APostProcessVolume>(Actor);
			if (PostProcess)
			{
				PostProcess->bEnabled = bVisible;
			}
		}
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

	SetActorsVisibility(true);

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

	SetActorsVisibility(false);

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
	bIsZoneActive = Data.bIsActive;
	bIsZoneActive ? CreateZone() : CureZone(nullptr);
}

void AENTWeakZone::OnElectricityMovementFinished()
{
	bIsZoneActive ? CureTimeline.Reverse() : CureTimeline.Play();
}
