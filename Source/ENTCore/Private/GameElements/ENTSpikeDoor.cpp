// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTSpikeDoor.h"
#include "AkComponent.h"
#include "ENTCameraShakeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavAreas/NavArea_Null.h"
#include "Player/ENTDefaultCharacter.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

AENTSpikeDoor::AENTSpikeDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	LeftFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftFrame"));
	LeftFrame->SetupAttachment(Root);
	LeftFrame->SetMobility(EComponentMobility::Static);

	RightFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightFrame"));
	RightFrame->SetupAttachment(Root);
	RightFrame->SetMobility(EComponentMobility::Static);

	InterMeshesA = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InterMeshesA"));
	InterMeshesA->SetupAttachment(Root);
	InterMeshesA->SetCanEverAffectNavigation(false);

	InterMeshesB = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InterMeshesB"));
	InterMeshesB->SetupAttachment(Root);
	InterMeshesB->SetCanEverAffectNavigation(false);

	NerveDoorNoises = CreateDefaultSubobject<UAkComponent>(TEXT("NerveDoorNoises"));
	NerveDoorNoises->SetupAttachment(Root);

	DoorNavModifier = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorNavModifier"));
	DoorNavModifier->SetupAttachment(Root);
	DoorNavModifier->bDynamicObstacle = true;
	DoorNavModifier->SetAreaClassOverride(UNavArea_Null::StaticClass());
}

void AENTSpikeDoor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector Dir = FVector::RightVector;
	FVector LeftPos = LeftFrame->GetRelativeLocation();
	FVector NewRightPos = LeftPos + Dir * DoorWidth;

	RightFrame->SetRelativeLocation(NewRightPos);

	ClearInterMeshes();
	GenerateInterMeshes();

	FVector BoxExtent = DoorNavModifier->GetUnscaledBoxExtent();
	float HalfSize = DoorWidth * 0.5f;

	BoxExtent.Y = HalfSize;

	DoorNavModifier->SetBoxExtent(BoxExtent);

	FVector RelativeLocation = DoorNavModifier->GetRelativeLocation();
	NavModifierDefaultLocation = FVector(RelativeLocation.X, HalfSize, RelativeLocation.Z);
	DoorNavModifier->SetRelativeLocation(NavModifierDefaultLocation);

	NavModifierOpenedLocation = NavModifierDefaultLocation;
	NavModifierOpenedLocation.Z += NavModifierHeightOffset;
}

void AENTSpikeDoor::BeginPlay()
{
	Super::BeginPlay();

	if (DropCurve)
	{
		FOnTimelineFloat UpdateEvent;
		FOnTimelineEvent FinishedEvent;

		UpdateEvent.BindDynamic(this, &AENTSpikeDoor::DropTimelineUpdate);
		FinishedEvent.BindDynamic(this, &AENTSpikeDoor::DropTimelineFinished);
		DropTimeline.AddInterpFloat(DropCurve, UpdateEvent);
		DropTimeline.SetTimelineFinishedFunc(FinishedEvent);
	}
}

void AENTSpikeDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DropTimeline.TickTimeline(DeltaTime);
}

void AENTSpikeDoor::GenerateInterMeshes()
{
	ClearInterMeshes();

	if (!InterMeshesA || !InterMeshesB || !InterMeshesA->GetStaticMesh() || !InterMeshesB->GetStaticMesh())
	{
		return;
	}

	if (!LeftFrame || !RightFrame)
	{
		return;
	}

	const FVector LeftRel = LeftFrame->GetRelativeLocation();
	const FVector RightRel = RightFrame->GetRelativeLocation();

	FVector Direction = (RightRel - LeftRel).GetSafeNormal();

	float MeshWidth = InterMeshesA->GetStaticMesh()->GetBoundingBox().GetSize().X * SpacingFactor;

	int32 NumMeshes = FMath::FloorToInt(DoorWidth / MeshWidth);
	if (NumMeshes <= 0) return;

	FVector Center = (LeftRel + RightRel) * 0.5f;
	float MaxDist = DoorWidth * 0.5f;

	const FBoxSphereBounds Bounds = InterMeshesA->GetStaticMesh()->GetBounds();
	const float MeshHeight = Bounds.BoxExtent.Z * 2.f;
	DropHeight = MeshHeight * 1.2f;

	for (int32 i = 0; i < NumMeshes; ++i)
	{
		bool bShouldChooseA = (i % 2 == 0);
		IsInterMeshA.Add(bShouldChooseA);

		UInstancedStaticMeshComponent* MeshToUse = bShouldChooseA ? InterMeshesA : InterMeshesB;
		if (!MeshToUse) continue;

		FVector PosRel = LeftRel + Direction * ((i + 0.5f) * MeshWidth);
		float RandomRotation = UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, 0.f, 360.f);
		FRotator RandRot(0.f, RandomRotation, 0.f);

		MeshToUse->AddInstance(FTransform(RandRot, PosRel, FVector::OneVector), false);

		if (InterMeshFX)
		{
			FVector FXPos = PosRel + FVector(-FXHeight, 0, 0);
			UNiagaraComponent* FX = UNiagaraFunctionLibrary::SpawnSystemAttached(InterMeshFX,MeshToUse, NAME_None, FXPos,RandRot,EAttachLocation::KeepRelativeOffset,false, false);
			InterMeshFXComponents.Add(FX);
		}

		InterInitialRotations.Add(RandRot);
		InterInitialRelativeLocations.Add(PosRel);

		float DistToCenter = FVector::Dist(PosRel, Center);
		float Weight = 1.f - FMath::Clamp(DistToCenter / MaxDist, 0.f, 1.f);
		float StartOffset = (1.f - Weight) * MaxStagger;
		InterStartOffsets.Add(StartOffset);
	}

	InterMeshesA->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	InterMeshesB->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AENTSpikeDoor::ClearInterMeshes()
{
	InterMeshesA->ClearInstances();
	InterMeshesB->ClearInstances();
	
	for (UNiagaraComponent* Comp : InterMeshFXComponents)
	{
		if (Comp) Comp->DestroyComponent();
	} 
	InterMeshFXComponents.Empty();

	IsInterMeshA.Empty();
	InterInitialRelativeLocations.Empty();
	InterStartOffsets.Empty();
	InterInitialRotations.Empty();
}

void AENTSpikeDoor::ToggleDoorState()
{
	bIsOpened ? CloseDoor() : OpenDoor();
}

void AENTSpikeDoor::OpenDoor()
{
	if (NerveDoorNoises)
	{
		NerveDoorNoises->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
	}

	if (OpenDuration > KINDA_SMALL_NUMBER)
	{
		DropTimeline.SetPlayRate(1.f / OpenDuration);
	}

	DropTimeline.Play();

	for (UNiagaraComponent* Comp : InterMeshFXComponents)
	{
		Comp->Activate();
	}
	bIsOpened = !bIsOpened;
}

void AENTSpikeDoor::CloseDoor()
{
	if (NerveDoorNoises)
	{
		NerveDoorNoises->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
	}

	if (CloseDuration > KINDA_SMALL_NUMBER)
	{
		DropTimeline.SetPlayRate(1.f / CloseDuration);
	}

	DoorNavModifier->SetRelativeLocation(NavModifierDefaultLocation);

	InterMeshesA->SetVisibility(true);
	InterMeshesB->SetVisibility(true);

	InterMeshesA->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	InterMeshesB->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	DropTimeline.Reverse();

	for (UNiagaraComponent* Comp : InterMeshFXComponents)
	{
		Comp->Activate();
	}
	
	bIsOpened = !bIsOpened;
}

void AENTSpikeDoor::DropTimelineUpdate(float Alpha)
{
	if (InterMeshesA->GetNumInstances() == 0 || InterMeshesB->GetNumInstances() == 0) return;

	int32 IndexA = 0;
	int32 IndexB = 0;

	for (int32 i = 0; i < IsInterMeshA.Num(); ++i)
	{
		int32 CurrentIndex;
		UInstancedStaticMeshComponent* MeshToUse;
		if (IsInterMeshA[i])
		{
			MeshToUse = InterMeshesA;
			CurrentIndex = IndexA++;
		}
		else
		{
			MeshToUse = InterMeshesB;
			CurrentIndex = IndexB++;
		}

		if (!MeshToUse) continue;

		const float StartOff = (i < InterStartOffsets.Num()) ? InterStartOffsets[i] : 0.f;

		float LocalT;
		if (Alpha <= StartOff)
		{
			LocalT = 0.f;
		}
		else
		{
			LocalT = (Alpha - StartOff) / (1.f - StartOff);
			LocalT = FMath::Clamp(LocalT, 0.f, 1.f);
		}

		float CurveValue = DropCurve ? DropCurve->GetFloatValue(LocalT) : LocalT;

		FTransform CurrentTransform;
		MeshToUse->GetInstanceTransform(CurrentIndex, CurrentTransform, false);

		FVector BaseRel = (i < InterInitialRelativeLocations.Num()) ? InterInitialRelativeLocations[i] : CurrentTransform.GetLocation();
		FVector NewRel = BaseRel;
		NewRel.Z = BaseRel.Z - DropHeight * CurveValue;

		FRotator BaseRot = (i < InterInitialRotations.Num())? InterInitialRotations[i] : CurrentTransform.GetRotation().Rotator();

		FRotator NewRot = BaseRot;
		NewRot.Yaw += 360.f * CurveValue;

		MeshToUse->UpdateInstanceTransform(CurrentIndex, FTransform(NewRot, NewRel, FVector::OneVector), false, false);
	}

	InterMeshesA->MarkRenderStateDirty();
	InterMeshesB->MarkRenderStateDirty();
}

void AENTSpikeDoor::DropTimelineFinished()
{
	if (bIsOpened)
	{
		DoorNavModifier->SetRelativeLocation(NavModifierOpenedLocation);
		InterMeshesA->SetVisibility(false);
		InterMeshesB->SetVisibility(false);

		InterMeshesA->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InterMeshesB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OnDoorOpened.Broadcast();
	}
	else
	{
		OnDoorClosed.Broadcast();
	}

	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Character)
	{
		return;
	}

	AENTDefaultCharacter* Player = Cast<AENTDefaultCharacter>(Character);
	if (!Player)
	{
		return;
	}

	Player->GetCameraShake()->MakeBigCameraShake();
}

void AENTSpikeDoor::Trigger_Implementation()
{
	IENTActivation::Trigger_Implementation();

	ToggleDoorState();
}

void AENTSpikeDoor::SetLock_Implementation(bool bState)
{
	IENTActivation::SetLock_Implementation(bState);

	Trigger_Implementation();
}

#if WITH_EDITOR
void AENTSpikeDoor::ClearDoor()
{
	TSet<UActorComponent*> Components = GetComponents();

	for (UActorComponent* Component : Components)
	{
		UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(Component);

		if (!StaticMeshComp)
		{
			continue;
		}

		if (StaticMeshComp == LeftFrame || StaticMeshComp == RightFrame || StaticMeshComp == InterMeshesA || StaticMeshComp == InterMeshesB)
		{
			continue;;
		}

		StaticMeshComp->DestroyComponent();
	}
}
#endif
