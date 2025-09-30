// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTWeakZone.h"
#include "AkGameplayStatics.h"
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

	Interactable = CreateDefaultSubobject<UENTInteractableComponent>(TEXT("Interactable"));
	Interactable->SetInteractionName(NSLOCTEXT("Actions", "DropAmber", "Drop Amber"));

	Foliage = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Foliage"));
	Foliage->SetupAttachment(BoxComponent);
	Foliage->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Foliage->SetMobility(EComponentMobility::Stationary);
}

void AENTWeakZone::BeginPlay()
{
	Super::BeginPlay();

	const UENTCoreConfig* CoreConfig =  GetDefault<UENTCoreConfig>();
	if (!CoreConfig)
	{
		return;
	}

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AENTWeakZone::OnZoneBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AENTWeakZone::OnZoneEndOverlap);

	for (const FENTInteractionPoints& InteractionPoint : InteractionPoints)
	{
		InteractionPoint.AmberMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InteractionPoint.InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		InteractionPoint.InteractionBox->SetCollisionResponseToChannel(CoreConfig->InteractionTraceChannel, ECR_Block);

		if (!InteractionPoint.bIsActive)
		{
			Interactable->AddInteractable(InteractionPoint.InteractionBox);
		}
	}

	Interactable->OnInteract.AddDynamic(this, &AENTWeakZone::OnInteract);

	// short delay because GetOverlappingActors does not work properly at the BeginPlay
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AENTWeakZone::InitZone, 0.2f, false);

#if WITH_EDITORONLY_DATA

	if (!Foliage)
	{
		return;
	}

	for (int32 i = 0; i < Foliage->GetInstanceCount(); i++)
	{
		FTransform InstanceTransform;
		Foliage->GetInstanceTransform(i, InstanceTransform, false);
		InstanceTransform.SetScale3D(FVector::ZeroVector);

		Foliage->UpdateInstanceTransform(i, InstanceTransform, false, false, false);
	}

	Foliage->MarkRenderStateDirty();

#endif

	FOnTimelineFloat UpdateEvent;

	UpdateEvent.BindDynamic(this, &AENTWeakZone::CureUpdate);
	CureTimeline.AddInterpFloat(CureCurve, UpdateEvent);
	CureTimeline.SetPlayRate(1/CureDuration);

	UpdateEvent.Unbind();

	UpdateEvent.BindDynamic(this, &AENTWeakZone::FoliageGrowthUpdate);
	FoliageTimeline.AddInterpFloat(FoliageGrowthCurve, UpdateEvent);
	FoliageTimeline.SetPlayRate(1/GrowthDuration);

	CheckIfEveryInteractionsPointActive();
}

void AENTWeakZone::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BoxComponent->SetBoxExtent(ZoneSize);
	Foliage->SetStaticMesh(FoliageMesh);
	Foliage->ClearInstances();

#if WITH_EDITORONLY_DATA
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
#endif

	for (FENTInteractionPoints& InteractionPoint : InteractionPoints)
	{
		if (!InteractionPoint.Mesh)
		{
			InteractionPoint.Mesh = DefaultInteractionPointMesh;
		}

		UActorComponent* DefaultComp = AddComponentByClass(UStaticMeshComponent::StaticClass(), true, InteractionPoint.Transform, false);
		UActorComponent* AmberComp = AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform::Identity, false);
		UActorComponent* BoxComp = AddComponentByClass(UBoxComponent::StaticClass(), true, FTransform::Identity, false);
		if (!DefaultComp || !AmberComp || !BoxComp)
		{
			continue;
		}

		UStaticMeshComponent* DefaultMeshComp = Cast<UStaticMeshComponent>(DefaultComp);
		UStaticMeshComponent* AmberMeshComp = Cast<UStaticMeshComponent>(AmberComp);
		UBoxComponent* InteractionComp = Cast<UBoxComponent>(BoxComp);
		if (!DefaultMeshComp || !AmberMeshComp || !InteractionComp)
		{
			continue;
		}

		InteractionPoint.MeshComp = DefaultMeshComp;
		InteractionPoint.AmberMeshComp = AmberMeshComp;
		InteractionPoint.InteractionBox = InteractionComp;

		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
		InteractionPoint.MeshComp->AttachToComponent(RootComponent, AttachmentRules);
		InteractionPoint.AmberMeshComp->AttachToComponent(InteractionPoint.MeshComp, AttachmentRules);
		InteractionPoint.InteractionBox->AttachToComponent(InteractionPoint.MeshComp, AttachmentRules);

		InteractionPoint.MeshComp->SetStaticMesh(InteractionPoint.Mesh);
		InteractionPoint.AmberMeshComp->SetStaticMesh(AmberMesh);
		InteractionPoint.InteractionBox->SetBoxExtent(InteractionBoxExtent, false);
		InteractionPoint.InteractionBox->SetRelativeLocation(FVector(0.0f, 0.0f, InteractionBoxExtent.Z));

		if (!Foliage)
		{
			continue;
		}

		FTransform WorldTransform = InteractionPoint.Transform * BoxComponent->GetComponentTransform();

		FVector Origin = WorldTransform.GetLocation();
		float OriginHeight = Origin.Z;
		Origin.Z += TraceLength;

		for (uint16 i = 0; i < MeshesNumberByInteractionsPoints; i++)
		{
			FVector Direction = UKismetMathLibrary::RandomUnitVectorFromStream(Seed);
			float DirectionLength = UKismetMathLibrary::RandomFloatInRangeFromStream(Seed, FoliageOffsetRange.GetLowerBoundValue(), FoliageOffsetRange.GetUpperBoundValue());

			FVector EndTrace = (Direction * DirectionLength) + Origin;
			EndTrace.Z = OriginHeight - TraceLength;

			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this);

			FHitResult Hit;
			bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, Origin, EndTrace, ObjectsTypes, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);

#if WITH_EDITORONLY_DATA
			if (bShowTraces)
			{
				UKismetSystemLibrary::DrawDebugPoint(this, Origin, TracesSize, FLinearColor::Green, INFINITY);
				UKismetSystemLibrary::DrawDebugPoint(this, EndTrace, TracesSize, FLinearColor::Green, INFINITY);
				UKismetSystemLibrary::DrawDebugLine(this, Origin, EndTrace, FLinearColor::Green, INFINITY);

				if (bHit)
				{
					UKismetSystemLibrary::DrawDebugPoint(this, Hit.Location, TracesSize, FLinearColor::Red, INFINITY);
				}
			}
#endif

			if (!bHit)
			{
				continue;
			}

			float RandomAngle = UKismetMathLibrary::RandomFloatInRangeFromStream(Seed, 0.0f, 360.0f);

			FVector TargetFoliageScale = FVector::ZeroVector;

#if WITH_EDITORONLY_DATA
			if (bShowFoliage)
			{
				TargetFoliageScale = FoliageScale;
			}
#endif

			FTransform FoliageTransform(FRotator(0.0f, RandomAngle, 0.0f), Hit.Location, TargetFoliageScale);
			Foliage->AddInstance(FoliageTransform, true);
		}
	}
}

void AENTWeakZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	for (FENTInteractionPoints& InteractionPoint : InteractionPoints)
	{
		if (!InteractionPoint.bIsActive)
		{
			continue;
		}

		FVector CurrentLocation = InteractionPoint.AmberMeshComp->GetRelativeLocation();
		FVector TargetLocation = FVector(0.0f, 0.0f, TargetAmberHeight);
		float Alpha = DeltaSeconds * AmberAnimSpeed;

		FVector ResultLocation = UKismetMathLibrary::VLerp(CurrentLocation, TargetLocation, Alpha);
		InteractionPoint.AmberMeshComp->SetRelativeLocation(ResultLocation);
	}

	CureTimeline.TickTimeline(DeltaSeconds);
	FoliageTimeline.TickTimeline(DeltaSeconds);
}

void AENTWeakZone::InitZone()
{
	TArray<AActor*> OverlappingActors;
	BoxComponent->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor == nullptr)
		{
			continue;
		}

		if(OverlappingActor->Implements<UENTWeakZoneInterface>())
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

		if(OverlappingActor->Implements<UENTWeakZoneInterface>())
		{
			IENTWeakZoneInterface::Execute_OnExitWeakZone(OverlappingActor);
		}
	}

	bIsZoneActive = false;
	BoxComponent->DestroyComponent();
}

void AENTWeakZone::CureUpdate(float Alpha)
{
	float ScalarParam = FMath::Lerp(1.0f, 0.0f, Alpha);
	MaterialBlackAndWhite->SetScalarParameterValue(CureParam, ScalarParam);
}

void AENTWeakZone::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->Implements<UENTWeakZoneInterface>())
	{
		IENTWeakZoneInterface::Execute_OnEnterWeakZone(OtherActor, bIsZoneActive);
	}
}

void AENTWeakZone::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->Implements<UENTWeakZoneInterface>())
	{
		IENTWeakZoneInterface::Execute_OnExitWeakZone(OtherActor);
	}
}

void AENTWeakZone::OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent)
{
	if (!Pawn)
	{
		return;
	}

	AENTDefaultCharacter* Character = Cast<AENTDefaultCharacter>(Pawn);
	if (!Character)
	{
		return;
	}

	if (!Character->HasRequiredQuantity(EAmberType::WeakAmber, CostByPoint))
	{
		return;
	}

	if (Character->GetStateMachine())
	{
		// play the spike animation
		Character->GetStateMachine()->ChangeState(EENTCharacterStateID::Anim);
	}

	UAkGameplayStatics::PostEvent(GrowlNoise, nullptr, 0, FOnAkPostEventCallback());
	Character->UseAmber(AmberType, CostByPoint);

	FENTInteractionPoints* InteractionPoint = FindInteractionPoint(InteractionComponent);
	if (!InteractionPoint)
	{
		return;
	}

	UAkGameplayStatics::PostEventAtLocation(InjectAmberNoise, InteractionPoint->Transform.GetLocation(), InteractionPoint->Transform.GetRotation().Rotator(), this);
	InteractionPoint->bIsActive = true;
	Interactable->RemoveInteractable(InteractionPoint->InteractionBox);

	CheckIfEveryInteractionsPointActive();
}

FENTInteractionPoints* AENTWeakZone::FindInteractionPoint(TObjectPtr<UPrimitiveComponent> Comp)
{
	for (FENTInteractionPoints& InteractionPoint : InteractionPoints)
	{
		if (InteractionPoint.InteractionBox == Comp)
		{
			return &InteractionPoint;
		}
	}

	return nullptr;
}

bool AENTWeakZone::IsEveryInteractionPointsActive() const
{
	for (const FENTInteractionPoints& InteractionPoint : InteractionPoints)
	{
		if (!InteractionPoint.bIsActive)
		{
			return false;
		}
	}

	return true;
}

void AENTWeakZone::CheckIfEveryInteractionsPointActive()
{
	if (!IsEveryInteractionPointsActive())
	{
		return;
	}

	CureTimeline.Play();
	FoliageTimeline.Play();

	for (const FENTInteractionPoints& InteractionPoint : InteractionPoints)
	{
		UAkGameplayStatics::PostEventAtLocation(FoliageGrowthNoise, InteractionPoint.Transform.GetLocation(), InteractionPoint.Transform.Rotator(), this);
	}

	DestroyZone();
}

void AENTWeakZone::FoliageGrowthUpdate(float Alpha)
{
	if (!Foliage)
	{
		return;
	}

	FVector TargetScale = UKismetMathLibrary::VLerp(FVector::ZeroVector, FoliageScale, Alpha);

	for (int32 i = 0; i < Foliage->GetInstanceCount(); i++)
	{
		FTransform InstanceTransform;
		Foliage->GetInstanceTransform(i, InstanceTransform, true);
		InstanceTransform.SetScale3D(TargetScale);

		Foliage->UpdateInstanceTransform(i, InstanceTransform, true, false, false);
	}

	Foliage->MarkRenderStateDirty();
}

FENTGameElementData& AENTWeakZone::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	FENTWeakZoneData Data = FENTWeakZoneData();

	Data.ActivatedInteractionPoints.Empty(InteractionPoints.Num());
	for (const FENTInteractionPoints& InteractionPoint : InteractionPoints)
	{
		Data.ActivatedInteractionPoints.Add(InteractionPoint.bIsActive);
	}

	return CurrentWorldSave->WeakZoneData.Add(GetName(), Data);
}

void AENTWeakZone::LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave)
{
	const FENTWeakZoneData& Data = static_cast<const FENTWeakZoneData&>(GameElementData);

	if (InteractionPoints.Num() != Data.ActivatedInteractionPoints.Num())
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("%s has a different number in the world and in the save, please clear your save file"), *GetActorLabel(false));

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	for (int i = 0; i < Data.ActivatedInteractionPoints.Num(); i++)
	{
		InteractionPoints[i].bIsActive = Data.ActivatedInteractionPoints[i];
	}
}
