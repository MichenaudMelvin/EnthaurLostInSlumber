// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTAmberOre.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "Components/BoxComponent.h"
#include "ENTInteractableComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/LightComponent.h"
#include "Engine/Light.h"
#include "GameElements/ENTNerveReceptacle.h"
#include "GameElements/ENTWeakZone.h"
#include "Interface/ENTActivation.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/States/ENTCharacterStateMachine.h"
#include "Saves/ENTPlayerSave.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTWorldSave.h"
#include "Subsystems/ENTPlayerSaveSubsystem.h"

AENTAmberOre::AENTAmberOre()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Static);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetMobility(EComponentMobility::Static);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	MeshInteraction = CreateDefaultSubobject<UBoxComponent>(TEXT("MeshInteraction"));
	MeshInteraction->SetupAttachment(Mesh);
	MeshInteraction->SetMobility(EComponentMobility::Static);
	MeshInteraction->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	AmberMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmberMesh"));
	AmberMesh->SetupAttachment(Mesh);
	AmberMesh->SetMobility(EComponentMobility::Movable);
	AmberMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Interactable = CreateDefaultSubobject<UENTInteractableComponent>(TEXT("Interactable"));
	Interactable->SetInteractionName(NSLOCTEXT("Actions", "PickAmber", "Pick Amber"));

	AmberOreNoises = CreateDefaultSubobject<UAkComponent>(TEXT("AmberOreNoises"));
	AmberOreNoises->SetupAttachment(Mesh);

	Foliage = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Foliage"));
	Foliage->SetupAttachment(Mesh);
	Foliage->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Foliage->SetMobility(EComponentMobility::Stationary);
}

void AENTAmberOre::BeginPlay()
{
	Super::BeginPlay();

	if (!bIsEmpty) Interactable->AddInteractable(MeshInteraction);
	Interactable->OnInteract.AddDynamic(this, &AENTAmberOre::OnInteract);

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

	if (!bIsLoaded)
	{
		bShouldFoliagePlay = bIsEmpty;
	}

	FOnTimelineFloat UpdateEvent;
	FOnTimelineEvent FinishedEvent;

	UpdateEvent.BindDynamic(this, &AENTAmberOre::FoliageGrowthUpdate);
	FinishedEvent.BindDynamic(this, &AENTAmberOre::FoliageGrowthFinished);
	FoliageTimeline.AddInterpFloat(FoliageGrowthCurve, UpdateEvent);
	FoliageTimeline.SetPlayRate(1 / GrowthDuration);
	FoliageTimeline.SetTimelineFinishedFunc(FinishedEvent);

	UpdateEvent.Unbind();

	UpdateEvent.BindDynamic(this, &AENTAmberOre::FillAmberUpdate);
	FillAmberTimeline.AddInterpFloat(FillAmberCurve, UpdateEvent);
	FillAmberTimeline.SetPlayRate(1 / FillAmberDuration);

	SetActorsVisibility(bIsEmpty);

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!PlayerCharacter)
	{
		return;
	}

	AENTDefaultCharacter* Player = Cast<AENTDefaultCharacter>(PlayerCharacter);
	if (!Player->OnAmberUpdate.IsAlreadyBound(this, &AENTAmberOre::OnPlayerAmberUpdate))
	{
		Player->OnAmberUpdate.AddDynamic(this, &AENTAmberOre::OnPlayerAmberUpdate);
	}
}

void AENTAmberOre::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Mesh->SetStaticMesh(SourceMesh);

	TargetAmberHeight = bIsEmpty ? EmptyAmberHeight : FullAmberHeight;
	FVector ResultLocation = AmberMesh->GetRelativeLocation();
	ResultLocation.Z = TargetAmberHeight;
	AmberMesh->SetRelativeLocation(ResultLocation);

	Foliage->SetStaticMesh(FoliageMesh);
	Foliage->ClearInstances();

#if WITH_EDITORONLY_DATA
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
#endif

	FilledLightsIntensity.Empty();
	for (TObjectPtr<ALight> Light : FilledLights)
	{
		if (!Light || !Light->GetLightComponent())
		{
			continue;
		}

		FilledLightsIntensity.Add(Light->GetLightComponent()->Intensity);
		Light->GetLightComponent()->SetVisibility(!bIsEmpty);
	}

	EmptyLightsIntensity.Empty();
	for (TObjectPtr<ALight> Light : EmptyLights)
	{
		if (!Light || !Light->GetLightComponent())
		{
			continue;
		}

		EmptyLightsIntensity.Add(Light->GetLightComponent()->Intensity);
		Light->GetLightComponent()->SetVisibility(bIsEmpty);
	}

	if (!Foliage)
	{
		return;
	}

	FVector Origin = GetTransform().GetLocation();
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

#if WITH_EDITOR
void AENTAmberOre::PostLoad()
{
	Super::PostLoad();

	SetActorsVisibility(false);
}

void AENTAmberOre::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	if (!PropertyAboutToChange)
	{
		return;
	}

	if (PropertyAboutToChange->NamePrivate == GET_MEMBER_NAME_CHECKED(AENTAmberOre, FilledActors))
	{
		SetFilledActorsVisibility(true);
	}
	else if (PropertyAboutToChange->NamePrivate == GET_MEMBER_NAME_CHECKED(AENTAmberOre, EmptyActors))
	{
		SetEmptyActorsVisibility(true);
	}
}

void AENTAmberOre::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName& ChangedProperty = PropertyChangedEvent.GetMemberPropertyName();

	if (ChangedProperty == GET_MEMBER_NAME_CHECKED(AENTAmberOre, FilledActors) || ChangedProperty == GET_MEMBER_NAME_CHECKED(AENTAmberOre, EmptyActors) || ChangedProperty == GET_MEMBER_NAME_CHECKED(AENTAmberOre, bIsEmpty))
	{
		SetActorsVisibility(bIsEmpty);
	}
}
#endif

void AENTAmberOre::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FoliageTimeline.TickTimeline(DeltaSeconds);
	FillAmberTimeline.TickTimeline(DeltaSeconds);
}

#pragma region LightsAndActors

void AENTAmberOre::SetActorsVisibility(bool bIsAmberEmpty) const
{
	SetFilledActorsVisibility(!bIsAmberEmpty);
	SetEmptyActorsVisibility(bIsAmberEmpty);
}

void AENTAmberOre::SetFilledActorsVisibility(bool bVisible) const
{
	SetArrayVisibility(bVisible, FilledActors);
}

void AENTAmberOre::SetEmptyActorsVisibility(bool bVisible) const
{
	SetArrayVisibility(bVisible, EmptyActors);
}

void AENTAmberOre::SetArrayVisibility(bool bVisible, const TArray<TObjectPtr<AActor>>& ActorArray) const
{
	for (TObjectPtr<AActor> Actor : ActorArray)
	{
		if (Actor && Actor->GetRootComponent())
		{
			USceneComponent* ActorRootComp = Actor->GetRootComponent();
			ActorRootComp->SetVisibility(bVisible, true);
		}
	}
}

#pragma endregion

void AENTAmberOre::OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent)
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

	if (bIsEmpty)
	{
		if (!Character->HasAmber())
		{
			return;
		}

		bIsEmpty = !bIsEmpty;
		UAkGameplayStatics::PostEvent(GrowlNoise, nullptr, 0, FOnAkPostEventCallback());
		Character->UseAmber();

		UAkGameplayStatics::PostEventAtLocation(InjectAmberNoise, GetTransform().GetLocation(), GetTransform().GetRotation().Rotator(), this);

		TargetAmberHeight = FullAmberHeight;
		Interactable->RemoveInteractable(MeshInteraction);
		SetActorsVisibility(bIsEmpty);
		FillAmberTimeline.PlayFromStart();
		OnFillAmber.Broadcast();

		UAkGameplayStatics::PostEventAtLocation(FoliageGrowthNoise, GetTransform().GetLocation(),GetTransform().Rotator(), this);
		FoliageTimeline.Play();
		TriggerFullLinkedObjects();
		TriggerEmptyLinkedObjects();
		if (LinkedWeakZone) LinkedWeakZone->CureZone(this);
	}
	else
	{
		if (Character->HasAmber())
		{
			return;
		}

		AmberOreNoises->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
		bIsEmpty = !bIsEmpty;
		Character->MineAmber();
		TargetAmberHeight = EmptyAmberHeight;
		TriggerEmptyLinkedObjects();
		TriggerFullLinkedObjects();
		Interactable->RemoveInteractable(MeshInteraction);
		SetActorsVisibility(bIsEmpty);
		FillAmberTimeline.PlayFromStart();
		FoliageTimeline.Reverse();
		OnEmptyAmber.Broadcast();

		if (LinkedWeakZone)LinkedWeakZone->CorruptZone(this);
	}
}

void AENTAmberOre::FillAmberUpdate(float Alpha)
{
	FVector CurrentLocation = AmberMesh->GetRelativeLocation();
	FVector TargetLocation = FVector(CurrentLocation.X, CurrentLocation.Y, TargetAmberHeight);

	FVector ResultLocation = FMath::Lerp(CurrentLocation, TargetLocation, Alpha);
	AmberMesh->SetRelativeLocation(ResultLocation);

	int FilledLightIndex = 0;
	for (TObjectPtr<ALight> Light : FilledLights)
	{
		if (!Light || !Light->GetLightComponent())
		{
			continue;
		}

		if (FilledLightsIntensity.IsValidIndex(FilledLightIndex))
		{
			float Intensity = FilledLightsIntensity[FilledLightIndex++];
			float StartValue = bIsEmpty ? Intensity : 0.0f;
			float TargetValue = bIsEmpty ? 0.0f : Intensity;
			Light->GetLightComponent()->SetIntensity(FMath::Lerp(StartValue, TargetValue, Alpha));
		}
	}

	int EmptyLightIndex = 0;
	for (TObjectPtr<ALight> Light : EmptyLights)
	{
		if (!Light || !Light->GetLightComponent())
		{
			continue;
		}

		if (EmptyLightsIntensity.IsValidIndex(EmptyLightIndex))
		{
			float Intensity = EmptyLightsIntensity[EmptyLightIndex++];
			float StartValue = bIsEmpty ? 0.0f : Intensity;
			float TargetValue = bIsEmpty ? Intensity : 0.0f;
			Light->GetLightComponent()->SetIntensity(FMath::Lerp(StartValue, TargetValue, Alpha));
		}
	}
}

void AENTAmberOre::OnPlayerAmberUpdate(bool bHasAmber)
{
	if (bHasAmber)
	{
		if (bIsEmpty) Interactable->AddInteractable(MeshInteraction);
		else Interactable->RemoveInteractable(MeshInteraction);
	}
	else
	{
		if (bIsEmpty) Interactable->RemoveInteractable(MeshInteraction);
		else Interactable->AddInteractable(MeshInteraction);
	}
}


void AENTAmberOre::FoliageGrowthUpdate(float Alpha)
{
	if (!Foliage || !bShouldFoliagePlay)
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

void AENTAmberOre::FoliageGrowthFinished()
{
	Interactable->AddInteractable(MeshInteraction);
}

void AENTAmberOre::TriggerLinkedObjects(TMap<AActor*, ENerveReactiveInteractionType> ObjectReactive)
{
	TArray<AActor*> Actors;
	ObjectReactive.GetKeys(Actors);

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

void AENTAmberOre::TriggerFullLinkedObjects()
{
	TriggerLinkedObjects(ObjectReactiveFull);
}

void AENTAmberOre::TriggerEmptyLinkedObjects()
{
	TriggerLinkedObjects(ObjectReactiveEmpty);
}

FENTGameElementData& AENTAmberOre::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	FENTAmberOreData Data;
	Data.bIsEmpty = bIsEmpty;

	return CurrentWorldSave->AmberOreData.Add(SaveID, Data);
}

void AENTAmberOre::LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave)
{
	bShouldFoliagePlay = bIsEmpty;

	const FENTAmberOreData& Data = static_cast<const FENTAmberOreData&>(GameElementData);

	bool bHasTheDefaultStatus = bIsEmpty == Data.bIsEmpty;

	bIsEmpty = Data.bIsEmpty;
	TargetAmberHeight = bIsEmpty ? EmptyAmberHeight : FullAmberHeight;
	FVector ResultLocation = AmberMesh->GetRelativeLocation();
	ResultLocation.Z = TargetAmberHeight;
	AmberMesh->SetRelativeLocation(ResultLocation);

	if (!bHasTheDefaultStatus)
	{
		TriggerEmptyLinkedObjects();
		TriggerFullLinkedObjects();

		if (bShouldFoliagePlay && !bIsEmpty)
		{
			FoliageTimeline.PlayFromStart();
		}
	}

	bIsLoaded = true;

	UENTPlayerSaveSubsystem* PlayerSaveSubsystem = GetGameInstance()->GetSubsystem<UENTPlayerSaveSubsystem>();
	if (!PlayerSaveSubsystem || !PlayerSaveSubsystem->GetPlayerSave())
	{
		return;
	}

	OnPlayerAmberUpdate(PlayerSaveSubsystem->GetPlayerSave()->bHasAmber);
}

void AENTAmberOre::FinishLoading(UENTWorldSave* LoadedWorldSave) {}
