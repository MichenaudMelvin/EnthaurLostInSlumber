// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTSpikeDoor.h"

#include "AkComponent.h"
#include "ENTCameraShakeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/ENTDefaultCharacter.h"

AENTSpikeDoor::AENTSpikeDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	LeftFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftFrame"));
	LeftFrame->SetupAttachment(Root);
	LeftFrame->SetMobility(EComponentMobility::Movable);

	RightFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightFrame"));
	RightFrame->SetupAttachment(Root);
	RightFrame->SetMobility(EComponentMobility::Movable);

	NerveDoorNoises = CreateDefaultSubobject<UAkComponent>(TEXT("NerveDoorNoises"));
	NerveDoorNoises->SetupAttachment(Root);
}

void AENTSpikeDoor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	ClearInterMeshes();
	GenerateInterMeshes();
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

float AENTSpikeDoor::GetDoorWidth() const
{
	if (!LeftFrame || !RightFrame) return 0.f;
	return FVector::Dist(LeftFrame->GetRelativeLocation(), RightFrame->GetRelativeLocation());
}

void AENTSpikeDoor::GenerateInterMeshes()
{
    ClearInterMeshes();

    if (!InterMeshA || !InterMeshB) return;
    if (!LeftFrame || !RightFrame) return;
	
    const FVector LeftRel = LeftFrame->GetRelativeLocation();
    const FVector RightRel = RightFrame->GetRelativeLocation();

    const float DoorWidth = FVector::Dist(LeftRel, RightRel);
    if (DoorWidth <= KINDA_SMALL_NUMBER) return;

    FVector Direction = (RightRel - LeftRel).GetSafeNormal();
	
    float MeshWidth = MeshSpacingOverride > 0.f ? MeshSpacingOverride : 0.f;
    if (MeshWidth <= KINDA_SMALL_NUMBER)
    {
        FVector SizeA = InterMeshA->GetBoundingBox().GetSize();
        if (SizeA.X > KINDA_SMALL_NUMBER)
        {
            MeshWidth = SizeA.X;
        }
        else
        {
            MeshWidth = 100.f;
        }
    }

    int32 NumMeshes = FMath::FloorToInt(DoorWidth / MeshWidth);
    if (NumMeshes <= 0) return;
	
    FVector Center = (LeftRel + RightRel) * 0.5f;
    float MaxDist = DoorWidth * 0.5f;

	if (InterMeshA)
	{
		const FBoxSphereBounds Bounds = InterMeshA->GetBounds();
		const float MeshHeight = Bounds.BoxExtent.Z * 2.f;
		DropHeight = MeshHeight * 1.2f;
	}

    for (int32 i = 0; i < NumMeshes; ++i)
    {
        UStaticMesh* MeshToUse = (i % 2 == 0) ? InterMeshA : InterMeshB;
        if (!MeshToUse) continue;
    	
        UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(this);
        MeshComp->SetStaticMesh(MeshToUse);
        MeshComp->SetMobility(EComponentMobility::Movable);
        MeshComp->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
        MeshComp->RegisterComponent();
    	
        FVector PosRel = LeftRel + Direction * ((i + 0.5f) * MeshWidth);
        MeshComp->SetRelativeLocation(PosRel);

    	float RandomRotation = UKismetMathLibrary::RandomFloatInRangeFromStream(RandomStream, 0.f, 360.f);
    	FRotator RandRot(0.f, RandomRotation, 0.f);
    	MeshComp->SetRelativeRotation(RandRot);
    	
        MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

        InterMeshes.Add(MeshComp);
    	InterInitialRotations.Add(RandRot);
        InterInitialRelativeLocations.Add(PosRel);
    	
        float DistToCenter = FVector::Dist(PosRel, Center);
        float Weight = 1.f - FMath::Clamp(DistToCenter / MaxDist, 0.f, 1.f);
        float StartOffset = (1.f - Weight) * MaxStagger;
        InterStartOffsets.Add(StartOffset);
    }
}

void AENTSpikeDoor::ClearInterMeshes()
{
	for (UStaticMeshComponent* Comp : InterMeshes)
	{
		if (Comp)
		{
			Comp->DestroyComponent();
		}
	}
	InterMeshes.Empty();
	InterInitialRelativeLocations.Empty();
	InterStartOffsets.Empty();
	InterInitialRotations.Empty();
}

void AENTSpikeDoor::DropTimelineUpdate(float Alpha)
{
	if (InterMeshes.Num() == 0) return;
	
	for (int32 i = 0; i < InterMeshes.Num(); ++i)
	{
		UStaticMeshComponent* MeshComp = InterMeshes[i];
		if (!MeshComp) continue;

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
		
		FVector BaseRel = (i < InterInitialRelativeLocations.Num()) ? InterInitialRelativeLocations[i] : MeshComp->GetRelativeLocation();
		FVector NewRel = BaseRel;
		NewRel.Z = BaseRel.Z - DropHeight * CurveValue;

		MeshComp->SetRelativeLocation(NewRel);

		FRotator BaseRot = (i < InterInitialRotations.Num())? InterInitialRotations[i] : MeshComp->GetRelativeRotation();

		FRotator NewRot = BaseRot;
		NewRot.Yaw += 360.f * CurveValue;

		MeshComp->SetRelativeRotation(NewRot);
	}
}

void AENTSpikeDoor::DropTimelineFinished()
{
	AENTDefaultCharacter* Player = Cast<AENTDefaultCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	Player->GetCameraShake()->MakeBigCameraShake();
}

void AENTSpikeDoor::Trigger_Implementation()
{
	IENTActivation::Trigger_Implementation();
	
	if (NerveDoorNoises)
	{
		NerveDoorNoises->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
	}
	if (!bIsOpened)
	{
		if (OpenDuration > KINDA_SMALL_NUMBER)
		{
			DropTimeline.SetPlayRate(1.f / OpenDuration);
		}
		DropTimeline.PlayFromStart();
	}
	else
	{
		if (CloseDuration > KINDA_SMALL_NUMBER)
		{
			DropTimeline.SetPlayRate(1.f / CloseDuration);
		}
		DropTimeline.ReverseFromEnd();
	}
	bIsOpened = !bIsOpened;
}

void AENTSpikeDoor::SetLock_Implementation(bool bState)
{
	IENTActivation::SetLock_Implementation(bState);
	Trigger_Implementation();
}

