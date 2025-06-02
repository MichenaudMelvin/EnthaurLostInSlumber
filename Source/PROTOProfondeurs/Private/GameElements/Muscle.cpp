// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/Muscle.h"

#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "Components/CameraShakeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InteractableComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/FirstPersonCharacter.h"
#include "Saves/WorldSaves/WorldSave.h"

#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Selection.h"
#endif

#pragma region Defaults

AMuscle::AMuscle()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Static);

	MuscleMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MuscleMeshComp->SetupAttachment(Root);
	MuscleMeshComp->SetMobility(EComponentMobility::Stationary);
	MuscleMeshComp->SetCollisionObjectType(ECC_WorldStatic);
	MuscleMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MuscleMeshComp->SetCollisionResponseToAllChannels(ECR_Block);

	ObjectsToCheck.Add(ObjectTypeQuery1);
	ObjectsToCheck.Add(ObjectTypeQuery2);
	ObjectsToCheck.Add(ObjectTypeQuery3);
	ObjectsToCheck.Add(ObjectTypeQuery4);

#if WITH_EDITORONLY_DATA
	TraceExtentVisibility = CreateDefaultSubobject<UBoxComponent>(TEXT("TraceExtent"));
	TraceExtentVisibility->SetupAttachment(MuscleMeshComp);
	TraceExtentVisibility->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TraceExtentVisibility->SetCollisionResponseToAllChannels(ECR_Ignore);
	TraceExtentVisibility->SetMobility(EComponentMobility::Stationary);
	TraceExtentVisibility->bIsEditorOnly = true;

	BounceDirectionTop = CreateDefaultSubobject<UArrowComponent>(TEXT("BounceDirectionTop"));
	BounceDirectionTop->SetupAttachment(MuscleMeshComp);
	BounceDirectionTop->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	BounceDirectionTop->bIsEditorOnly = true;

	BounceDirectionBack = CreateDefaultSubobject<UArrowComponent>(TEXT("BounceDirectionBack"));
	BounceDirectionBack->SetupAttachment(MuscleMeshComp);
	BounceDirectionBack->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	BounceDirectionBack->bIsEditorOnly = true;
#endif

	SpikeInteraction = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikeInteraction"));
	SpikeInteraction->SetupAttachment(Root);
	SpikeInteraction->SetCollisionObjectType(ECC_WorldStatic);
	SpikeInteraction->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SpikeInteraction->SetCollisionResponseToAllChannels(ECR_Block);
	SpikeInteraction->SetMobility(EComponentMobility::Static);

	MuscleDeformationNoises = CreateDefaultSubobject<UAkComponent>(TEXT("MuscleDeformationNoises"));
	MuscleDeformationNoises->SetupAttachment(MuscleMeshComp);

	SpikeInteractionNoises = CreateDefaultSubobject<UAkComponent>(TEXT("SpikeInteractionNoises"));
	SpikeInteractionNoises->SetupAttachment(SpikeInteraction);

	Interactable = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));

	SolidMuscleInteraction = NSLOCTEXT("Interactions", "SolidMuscleInteraction", "Make soft");
	SoftMuscleInteraction = NSLOCTEXT("Interactions", "SoftMuscleInteraction", "Make solid");

#if WITH_EDITORONLY_DATA
	USelection::SelectObjectEvent.AddUObject(this, &AMuscle::OnSelectionUpdate);
	USelection::SelectionChangedEvent.AddUObject(this, &AMuscle::OnSelectionUpdate);
#endif
}

void AMuscle::BeginPlay()
{
	Super::BeginPlay();

	bDefaultSolidity = bIsSolid;
	CurrentZScale = MuscleHeight.GetUpperBoundValue();

	DeformationSpeed = 1 / DeformationSpeed;

	bTriggerDeformation = false;

	if (bAllowInteraction)
	{
		Interactable->AddInteractable(SpikeInteraction);
		Interactable->OnInteract.AddDynamic(this, &AMuscle::Interact);
	}
	else
	{
		TArray<USceneComponent*> ChildrenComps;
		SpikeInteraction->GetChildrenComponents(true, ChildrenComps);
		for (USceneComponent* Child : ChildrenComps)
		{
			Child->DestroyComponent(false);
		}

		SpikeInteraction->DestroyComponent(false);
	}

	if (MuscleStateTransitionCurve)
	{
		FOnTimelineFloat UpdateEvent;
		UpdateEvent.BindDynamic(this, &AMuscle::UpdateMuscleStateTransition);
		MuscleStateTransitionTimeline.AddInterpFloat(MuscleStateTransitionCurve, UpdateEvent);
		MuscleStateTransitionTimeline.SetPlayRate(1/MuscleStateTransitionDuration);
	}

	FBox BoundingBox = MuscleMeshComp->GetStaticMesh()->GetBoundingBox();
	FVector MeshSize = BoundingBox.Max - BoundingBox.Min;
	MuscleZUnit = MeshSize.Z * 0.5f;

	if (!DynamicMaterial)
	{
		DynamicMaterial = MuscleMeshComp->CreateDynamicMaterialInstance(0, MuscleMaterial);
		MuscleMeshComp->SetMaterial(0, DynamicMaterial);
	}

	MuscleMeshComp->OnComponentHit.AddDynamic(this, &AMuscle::HitMuscleMesh);

	UpdateMuscleSolidity();

#if WITH_EDITORONLY_DATA
	if (bDrawInRunTime)
	{
		DrawProjections();
	}
#endif
}

void AMuscle::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	bDefaultSolidity = bIsSolid;
	CurrentZScale = MuscleHeight.GetUpperBoundValue();

	SpikeInteraction->SetVisibility(bAllowInteraction, true);

	if (!MuscleMeshComp)
	{
		return;
	}

	MuscleMeshComp->SetStaticMesh(MuscleMesh);

	RebuildMuscleMesh();

	if (!DynamicMaterial)
	{
		DynamicMaterial = MuscleMeshComp->CreateDynamicMaterialInstance(0, MuscleMaterial);
		MuscleMeshComp->SetMaterial(0, DynamicMaterial);
	}

	UpdateMuscleStateTransition(bIsSolid ? 0.0f : 1.0f);

#if WITH_EDITORONLY_DATA
	if (SelectedInEditor)
	{
		DrawProjections();
	}
#endif
}

void AMuscle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MuscleStateTransitionTimeline.TickTimeline(DeltaTime);
	DeformMesh(DeltaTime);
}

#pragma endregion

#pragma region Deformation

void AMuscle::StartDeformation()
{
	DeformationDirection = 1;
	bTriggerDeformation = true;
}

void AMuscle::EndDeformation()
{
	DeformationDirection = -1;
	bTriggerDeformation = true;
}

void AMuscle::DeformMesh(float DeltaTime)
{
	if (!DeformationCurve)
	{
		return;
	}

	if (bTriggerDeformation && !bIsSolid)
	{
		DeformationAlpha = (DeltaTime * DeformationDirection * DeformationSpeed) + DeformationAlpha;
		DeformationAlpha = FMath::Clamp(DeformationAlpha, 0, DeformationDuration);

		float Time = UKismetMathLibrary::NormalizeToRange(DeformationAlpha, 0.0f, DeformationDuration);
		float Alpha = DeformationCurve->GetFloatValue(Time);
		CurrentZScale = FMath::Lerp(MuscleHeight.GetUpperBoundValue(), MuscleHeight.GetLowerBoundValue(), Alpha);

		CurrentZScale = FMath::Clamp(CurrentZScale, MinPossibleScale, INFINITY);

		RebuildMuscleMesh();

		if (DeformationAlpha == 0.0f || DeformationAlpha == DeformationDuration)
		{
			bTriggerDeformation = false;

			EndDeformation();
		}
	}
	else
	{
		if (!IsDeformed())
		{
			EndDeformation();
		}
	}
}

bool AMuscle::IsDeformed() const
{
	return CurrentZScale != MuscleHeight.GetUpperBoundValue();
}

void AMuscle::RebuildMuscleMesh() const
{
	if (!MuscleMeshComp)
	{
		return;
	}

	MuscleMeshComp->SetRelativeScale3D(FVector(MuscleSize.X, MuscleSize.Y, CurrentZScale));

#if WITH_EDITORONLY_DATA
	FBox BoundingBox = MuscleMeshComp->GetStaticMesh()->GetBoundingBox();
	FVector MeshSize = (BoundingBox.Max - BoundingBox.Min) * MuscleMeshComp->GetRelativeScale3D();
	MeshSize *= 0.5f;

	if (BounceDirectionTop)
	{
		BounceDirectionTop->SetRelativeLocation(FVector(0.0f, 0.0f, (MeshSize.Z / CurrentZScale)));
	}

	if (BounceDirectionBack)
	{
		BounceDirectionBack->SetRelativeLocation(FVector(0.0f, 0.0f, -(MeshSize.Z / CurrentZScale)));
	}

	if (TraceExtentVisibility)
	{
		TraceExtentVisibility->SetWorldScale3D(FVector::OneVector);
		TraceExtentVisibility->SetBoxExtent(MeshSize + (TraceExtent * MuscleMeshComp->GetRelativeScale3D() * 0.5f));
	}
#endif
}

void AMuscle::UpdateMuscleSolidity()
{
	if (bIsSolid)
	{
		MuscleMeshComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Default, 0.0f));
		Interactable->SetInteractionName(SolidMuscleInteraction);
		MuscleStateTransitionTimeline.Reverse();
		MuscleDeformationNoises->PostAkEvent(SolidMuscleNoise);
		SpikeInteractionNoises->PostAkEvent(ToSolidInteractionNoise);
	}
	else
	{
		MuscleMeshComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.0f));
		Interactable->SetInteractionName(SoftMuscleInteraction);
		MuscleStateTransitionTimeline.Play();
		MuscleDeformationNoises->PostAkEvent(SoftMuscleNoise);
		SpikeInteractionNoises->PostAkEvent(ToSoftInteractionNoise);
	}
}

#pragma endregion

#pragma region Physics

void AMuscle::ToggleMuscleSolidity()
{
	if (bIsLocked)
	{
		return;
	}

	bIsSolid = !bIsSolid;
	UpdateMuscleSolidity();
}

void AMuscle::HitMuscleMesh(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HitMuscle(OtherActor, OtherComp);
}

void AMuscle::HitMuscle(AActor* HitActor, UPrimitiveComponent* OtherComp)
{
	if (!HitActor)
	{
		return;
	}

	if (bIsSolid)
	{
		UpdateMuscleSolidity();
		return;
	}

#if WITH_EDITORONLY_DATA
	if (bDrawHitCapsule && HitActor->IsA(ACharacter::StaticClass()))
	{
		ACharacter* Character = Cast<ACharacter>(HitActor);
		UCapsuleComponent* CapsuleComponent = Character->GetCapsuleComponent();
		UKismetSystemLibrary::DrawDebugCapsule(this, Character->GetActorLocation(), CapsuleComponent->GetScaledCapsuleHalfHeight(), CapsuleComponent->GetScaledCapsuleRadius(), FRotator::ZeroRotator, FLinearColor::Red, 15.0f, 5.0f);
	}
#endif

	if (!IsActorInRange(HitActor))
	{
		return;
	}

	UpdateMuscleSolidity();

	float ActorVelocityLength = OtherComp ? OtherComp->GetComponentVelocity().Size() : HitActor->GetVelocity().Size();

#if WITH_EDITORONLY_DATA
	if (bDrawHitCapsule)
	{
		const FString Message = FString::Printf(TEXT("ActorVelocity: %f, MinVelocity: %f"), ActorVelocityLength, MinTriggerVelocity);

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, Message);
		FMessageLog("BlueprintLog").Message(EMessageSeverity::Info, FText::FromString(Message));
	}
#endif

	if (ActorVelocityLength < MinTriggerVelocity)
	{
		return;
	}

	ActorVelocityLength = FMath::Clamp(ActorVelocityLength, MinTriggerVelocity, MaxLaunchVelocity);

	UAkGameplayStatics::PostEventAtLocation(BounceNoise, HitActor->GetActorLocation(), HitActor->GetActorRotation(), HitActor);

	StartDeformation();

	FVector LaunchVelocity = MuscleMeshComp->GetUpVector() * (bUseFixedVelocity ? FixedVelocity : (ActorVelocityLength * VelocityMultiplier));

	AFirstPersonCharacter* Player = Cast<AFirstPersonCharacter>(HitActor);
	if (Player)
	{
		Player->EjectCharacter(LaunchVelocity, true);
	}
	else
	{
		UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(HitActor->GetRootComponent());
		if (Primitive)
		{
			Primitive->AddForce(LaunchVelocity);
		}
	}
}

bool AMuscle::IsActorInRange(AActor* Actor)
{
	if (!Actor)
	{
		return false;
	}

	FVector MuscleLocation = MuscleMeshComp->GetComponentLocation();
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	TArray<FHitResult> HitResults;

	FBox BoundingBox = MuscleMeshComp->GetStaticMesh()->GetBoundingBox();
	FVector MeshSize = (BoundingBox.Max - BoundingBox.Min) * MuscleMeshComp->GetRelativeScale3D();
	MeshSize *= 0.5;

	FVector TraceSize = MeshSize + (TraceExtent * MuscleMeshComp->GetRelativeScale3D() * 0.5f);

	EDrawDebugTrace::Type DrawDebugTrace = EDrawDebugTrace::None;

#if WITH_EDITORONLY_DATA
	if (bDrawHitCapsule)
	{
		DrawDebugTrace = EDrawDebugTrace::ForDuration;
	}
#endif

	bool bHit = UKismetSystemLibrary::BoxTraceMultiForObjects(this, MuscleLocation, MuscleLocation, TraceSize, MuscleMeshComp->GetComponentRotation(), ObjectsToCheck, false, ActorsToIgnore, DrawDebugTrace, HitResults, true, FLinearColor::Red, FLinearColor::Green, 15.0f);

	if (!bHit)
	{
		return false;
	}

	for (const FHitResult& HitResult : HitResults)
	{
		if (HitResult.GetActor() == Actor)
		{
			return true;
		}
	}

	return false;
}

#pragma endregion

#pragma region Appearance

void AMuscle::UpdateMuscleStateTransition(float Alpha)
{
	if (!DynamicMaterial)
	{
		return;
	}

	DynamicMaterial->SetScalarParameterValue(MuscleStateTransitionParam, Alpha);
}

#pragma endregion

#pragma region Interaction

void AMuscle::Interact(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractComponent)
{
	Cast<AFirstPersonCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))->GetCameraShake()->MakeSmallCameraShake();

	UAkGameplayStatics::PostEventAtLocation(InteractionEvent, SpikeInteraction->GetComponentLocation(), SpikeInteraction->GetComponentRotation(), this);

	ToggleMuscleSolidity();
}

#pragma endregion

#pragma region Interfaces

void AMuscle::OnActorAbove_Implementation(AActor* Actor)
{
	IGroundAction::OnActorAbove_Implementation(Actor);

	HitMuscle(Actor, nullptr);
}

void AMuscle::OnEnterWeakZone_Implementation(bool bIsZoneActive)
{
	IWeakZoneInterface::OnEnterWeakZone_Implementation(bIsZoneActive);

	if (bIsZoneActive && Interactable->OnInteract.IsAlreadyBound(this, &AMuscle::Interact))
	{
		Interactable->OnInteract.RemoveDynamic(this, &AMuscle::Interact);
	}
}

void AMuscle::OnExitWeakZone_Implementation()
{
	IWeakZoneInterface::OnExitWeakZone_Implementation();

	if (!Interactable->OnInteract.IsAlreadyBound(this, &AMuscle::Interact))
	{
		Interactable->OnInteract.AddDynamic(this, &AMuscle::Interact);
	}
}

void AMuscle::Trigger_Implementation()
{
	INerveReactive::Trigger_Implementation();

	ToggleMuscleSolidity();
}

void AMuscle::SetLock_Implementation(bool state)
{
	INerveReactive::SetLock_Implementation(state);

	if (bIsSolid == bDefaultSolidity)
	{
		return;
	}

	Trigger();
	bIsLocked = state;
}

#pragma endregion

#pragma region Save

FGameElementData& AMuscle::SaveGameElement(UWorldSave* CurrentWorldSave)
{
	FMuscleData Data;
	Data.bIsSolid = bIsSolid;

	return CurrentWorldSave->MuscleData.Add(GetName(), Data);
}

void AMuscle::LoadGameElement(const FGameElementData& GameElementData)
{
	const FMuscleData& Data = static_cast<const FMuscleData&>(GameElementData);
	bIsSolid = Data.bIsSolid;

	UpdateMuscleSolidity();
}

#pragma endregion

#pragma region Debug

#if WITH_EDITORONLY_DATA

void AMuscle::OnSelectionUpdate(UObject* Object)
{
	if (Object == this && !SelectedInEditor)
	{
		SelectedInEditor = true;
		DrawProjections();
	}
	else if (SelectedInEditor && !IsSelected())
	{
		SelectedInEditor = false;
		ClearProjectionDraw();
	}
}

void AMuscle::DrawProjections()
{
	ClearProjectionDraw();

	UCharacterFallState* FallStateObject = CharacterFallStateClass->GetDefaultObject<UCharacterFallState>();
	if (!FallStateObject)
	{
		return;
	}

	float PlayerGravity = GetWorld()->GetGravityZ() * FallStateObject->GetGravityScale();

	FPredictProjectilePathParams DefaultParams;
	DefaultParams.bTraceWithCollision = true;
	DefaultParams.bTraceComplex = false;
	DefaultParams.ActorsToIgnore.Add(this);
	DefaultParams.DrawDebugType = EDrawDebugTrace::None;
	DefaultParams.MaxSimTime = MaxSimTime;
	DefaultParams.SimFrequency = SimFrequency;
	DefaultParams.OverrideGravityZ = PlayerGravity;
	DefaultParams.ObjectTypes.Add(ObjectTypeQuery1);
	DefaultParams.ObjectTypes.Add(ObjectTypeQuery2);

	FVector Center = TraceExtentVisibility->GetComponentLocation();
	FVector Extent = TraceExtentVisibility->GetScaledBoxExtent();

	FVector UpperPointLeft = Center;
	UpperPointLeft -= Extent.Y * MuscleMeshComp->GetRightVector();
	UpperPointLeft += Extent.X * MuscleMeshComp->GetForwardVector();

	FVector UpperPointRight = Center;
	UpperPointRight += Extent.Y * MuscleMeshComp->GetRightVector();
	UpperPointRight += Extent.X * MuscleMeshComp->GetForwardVector();

	FVector LowerPointLeft = Center;
	LowerPointLeft -= Extent.Y * MuscleMeshComp->GetRightVector();
	LowerPointLeft -= Extent.X * MuscleMeshComp->GetForwardVector();

	FVector LowerPointRight = Center;
	LowerPointRight += Extent.Y * MuscleMeshComp->GetRightVector();
	LowerPointRight -= Extent.X * MuscleMeshComp->GetForwardVector();

	if (bUseFixedVelocity)
	{
		FVector ForwardVelocity = (MuscleMeshComp->GetUpVector() * FixedVelocity);
		FVector BackwardVelocity = ForwardVelocity * -1;

		DrawSingleProjection(DefaultParams, UpperPointLeft, ForwardVelocity, MinVelocityColor);
		DrawSingleProjection(DefaultParams, UpperPointRight, ForwardVelocity, MinVelocityColor);
		DrawSingleProjection(DefaultParams, LowerPointLeft, ForwardVelocity, MinVelocityColor);
		DrawSingleProjection(DefaultParams, LowerPointRight, ForwardVelocity, MinVelocityColor);

		DrawSingleProjection(DefaultParams, UpperPointLeft, BackwardVelocity, MinVelocityColor);
		DrawSingleProjection(DefaultParams, UpperPointRight, BackwardVelocity, MinVelocityColor);
		DrawSingleProjection(DefaultParams, LowerPointLeft, BackwardVelocity, MinVelocityColor);
		DrawSingleProjection(DefaultParams, LowerPointRight, BackwardVelocity, MinVelocityColor);
	}
	else
	{
		FVector MinForwardVelocity = (MuscleMeshComp->GetUpVector() * MinTriggerVelocity * VelocityMultiplier);
		FVector MaxForwardVelocity = (MuscleMeshComp->GetUpVector() * MaxLaunchVelocity);

		FVector MinBackwardVelocity = MinForwardVelocity * -1;
		FVector MaxBackwardVelocity = MaxForwardVelocity * -1;

		DrawSingleProjection(DefaultParams, UpperPointLeft, MinForwardVelocity, MinVelocityColor);
		DrawSingleProjection(DefaultParams, UpperPointRight, MinForwardVelocity, MinVelocityColor);
		DrawSingleProjection(DefaultParams, LowerPointLeft, MinForwardVelocity, MinVelocityColor);
		DrawSingleProjection(DefaultParams, LowerPointRight, MinForwardVelocity, MinVelocityColor);

		DrawSingleProjection(DefaultParams, UpperPointLeft, MaxForwardVelocity, MaxVelocityColor);
		DrawSingleProjection(DefaultParams, UpperPointRight, MaxForwardVelocity, MaxVelocityColor);
		DrawSingleProjection(DefaultParams, LowerPointLeft, MaxForwardVelocity, MaxVelocityColor);
		DrawSingleProjection(DefaultParams, LowerPointRight, MaxForwardVelocity, MaxVelocityColor);

		DrawSingleProjection(DefaultParams, UpperPointLeft, MinBackwardVelocity, MinVelocityColor);
		DrawSingleProjection(DefaultParams, UpperPointRight, MinBackwardVelocity, MinVelocityColor);
		DrawSingleProjection(DefaultParams, LowerPointLeft, MinBackwardVelocity, MinVelocityColor);
		DrawSingleProjection(DefaultParams, LowerPointRight, MinBackwardVelocity, MinVelocityColor);

		DrawSingleProjection(DefaultParams, UpperPointLeft, MaxBackwardVelocity, MaxVelocityColor);
		DrawSingleProjection(DefaultParams, UpperPointRight, MaxBackwardVelocity, MaxVelocityColor);
		DrawSingleProjection(DefaultParams, LowerPointLeft, MaxBackwardVelocity, MaxVelocityColor);
		DrawSingleProjection(DefaultParams, LowerPointRight, MaxBackwardVelocity, MaxVelocityColor);
	}
}

void AMuscle::ClearProjectionDraw() const
{
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
}

void AMuscle::DrawSingleProjection(const FPredictProjectilePathParams& Params, const FVector& StartPoint, const FVector& Velocity, const FColor& DrawColor) const
{
	FPredictProjectilePathParams TargetParams(Params);
	TargetParams.StartLocation = StartPoint;
	TargetParams.LaunchVelocity = Velocity;

	FPredictProjectilePathResult PredictResult;
	bool bHit = UGameplayStatics::PredictProjectilePath(this, TargetParams, PredictResult);

	AFirstPersonCharacter* PlayerObject = FirstPersonCharacterClass->GetDefaultObject<AFirstPersonCharacter>();
	if (!PlayerObject)
	{
		return;
	}

	float DrawRadius = PlayerObject->GetCapsuleComponent()->GetScaledCapsuleRadius();

	for (const FPredictProjectilePathPointData& PathPt : PredictResult.PathData)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, PathPt.Location, DrawRadius, 12, DrawColor, INFINITY);
	}

	if (bHit)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, PredictResult.HitResult.Location, DrawRadius + 1.0f, 12, HitColor, INFINITY);
	}
}

#endif

#pragma endregion
