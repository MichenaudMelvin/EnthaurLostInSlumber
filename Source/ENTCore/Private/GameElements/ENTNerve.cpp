// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTNerve.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "ENTInteractableComponent.h"
#include "GameFramework/Character.h"
#include "GameElements/ENTNerveReceptacle.h"
#include "Components/ENTPhysicConstraint.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Config/ENTCoreConfig.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"
#include "Saves/WorldSaves/ENTWorldSave.h"
#include "Player/ENTDefaultCharacter.h"
#include "Saves/WorldSaves/ENTGameElementData.h"

AENTNerve::AENTNerve()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Static);

	SplineCable = CreateDefaultSubobject<USplineComponent>(TEXT("SplineCable"));
	SplineCable->SetupAttachment(RootComponent);
	SplineCable->SetMobility(EComponentMobility::Static);

	USplineMeshComponent* SplineMesh = CreateDefaultSubobject<USplineMeshComponent>(TEXT("Spline Mesh"));
	SplineMesh->SetMobility(EComponentMobility::Movable);
	SplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SplineMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SplineMesh->SetGenerateOverlapEvents(false);
	SplineMeshes.Add(SplineMesh);

	CorruptNerveBlocker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Corruption"));
	CorruptNerveBlocker->SetupAttachment(RootComponent);

	NerveBall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	NerveBall->SetupAttachment(RootComponent);

	NerveStretchComp = CreateDefaultSubobject<UAkComponent>(TEXT("NerveStretchComp"));
	NerveStretchComp->SetupAttachment(NerveBall);

	InteractableComponent = CreateDefaultSubobject<UENTInteractableComponent>(TEXT("Interaction"));
	InteractableComponent->OnInteract.AddDynamic(this, &AENTNerve::Interaction);

	CableColliders.Add(ObjectTypeQuery1);
	CableColliders.Add(ObjectTypeQuery2);
}

void AENTNerve::BeginPlay()
{
	Super::BeginPlay();

	InteractableComponent->AddInteractable(NerveBall);
	DefaultNervePosition = NerveBall->GetComponentLocation();

	if (CorruptNerveBlocker)
	{
		UMaterialInterface* Mat = CorruptNerveBlocker->GetMaterial(0);
		if (Mat)
		{
			CorruptMID = UMaterialInstanceDynamic::Create(Mat, this);
			CorruptNerveBlocker->SetMaterial(0, CorruptMID);
		}
	}

	//TargetMesh = (bIsLigament) ? LigamentMesh : NerveMesh;

	FOnTimelineFloat UpdateEvent;
	FOnTimelineEvent FinishEvent;
	
	UpdateEvent.BindDynamic(this, &AENTNerve::RetractCable);
	FinishEvent.BindDynamic(this, &AENTNerve::FinishRetractCable);
	RetractTimeline.AddInterpFloat(RetractionCurve, UpdateEvent);
	RetractTimeline.SetTimelineFinishedFunc(FinishEvent);

	UpdateEvent.Unbind();

	UpdateEvent.BindDynamic(this, &AENTNerve::UpdateEnterWeakZone);
	EnterWeakZoneTimeline.AddInterpFloat(EnterWeakZoneCurve, UpdateEvent);
	EnterWeakZoneTimeline.SetPlayRate(1 / RetractLigamentDuration);

	UpdateEvent.Unbind();

	if (StretchedLigamentMaterial)
	{
		DynamicStretchedLigamentMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, StretchedLigamentMaterial);
		DynamicStretchedLigamentMaterial->SetScalarParameterValue(FName("TransparencyDistance"), TransparencyDistance);
	}
}

void AENTNerve::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	TargetMesh = (bIsLigament) ? LigamentMesh : NerveMesh;

	NerveBall -> SetStaticMesh((bIsLigament) ? LigamentBallMesh : NerveBallMesh);

	if (bIsLigament)
	{
		CorruptNerveBlocker->SetVisibility(false);
	}
	else
	{
		CorruptNerveBlocker -> SetStaticMesh(CorruptNerveBlockerMesh);
	}

	FVector CableMeshSize = TargetMesh->GetBoundingBox().Max - TargetMesh->GetBoundingBox().Min;
	switch (CableForwardAxis)
	{
	case ESplineMeshAxis::X:
		SingleCableLength = CableMeshSize.X;
		break;
	case ESplineMeshAxis::Y:
		SingleCableLength = CableMeshSize.Y;
		break;
	case ESplineMeshAxis::Z:
		SingleCableLength = CableMeshSize.Z;
		break;
	}

	ResetCables(false);

	if (StartCableLength > CableMaxExtension)
	{
		CableMaxExtension = StartCableLength + 1000.0f;
	}
}

#if WITH_EDITOR
void AENTNerve::PostInitProperties()
{
	Super::PostInitProperties();

	if (!NerveMesh || !LigamentMesh)
	{
		return;
	}
}

void AENTNerve::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.GetPropertyName();

	if ((TargetMesh && PropertyName == GET_MEMBER_NAME_CHECKED(AENTNerve, TargetMesh)) || PropertyName == GET_MEMBER_NAME_CHECKED(AENTNerve, CableForwardAxis))
	{
		FVector CableMeshSize = TargetMesh->GetBoundingBox().Max - TargetMesh->GetBoundingBox().Min;
		switch (CableForwardAxis)
		{
			case ESplineMeshAxis::X:
				SingleCableLength = CableMeshSize.X;
				break;
			case ESplineMeshAxis::Y:
				SingleCableLength = CableMeshSize.Y;
				break;
			case ESplineMeshAxis::Z:
				SingleCableLength = CableMeshSize.Z;
				break;
		}
	}
}
#endif

void AENTNerve::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ApplyCablesPhysics();
	EnterWeakZoneTimeline.TickTimeline(DeltaSeconds);
	if (RetractTimeline.IsPlaying())
	{
		RetractTimeline.TickTimeline(DeltaSeconds);
	}
}

#pragma region Cables

void AENTNerve::AddSplinePoint(const FVector& SpawnLocation, const ESplineCoordinateSpace::Type& CoordinateSpace, bool bAutoCorrect) const
{
	int Index = SplineCable->GetNumberOfSplinePoints();
	SplineCable->AddSplinePoint(SpawnLocation, CoordinateSpace, false);
	SplineCable->SetTangentAtSplinePoint(Index, FVector::ZeroVector, ESplineCoordinateSpace::Local, false);

	// correct the location of the last spline point
	int LastIndex = Index - 1;
	if (LastIndex > 0 && bAutoCorrect)
	{
		SplineCable->SetLocationAtSplinePoint(LastIndex, SpawnLocation, CoordinateSpace, false);
	}

	SplineCable->UpdateSpline();
}

void AENTNerve::RemoveLastSplinePoint() const
{
	int32 LastSplinePointIndex = SplineCable->GetNumberOfSplinePoints() - 1;
	SplineCable->RemoveSplinePoint(LastSplinePointIndex, true);
}

void AENTNerve::AddSplineMesh(bool bMakeNoise)
{
	
	UActorComponent* Comp = AddComponentByClass(USplineMeshComponent::StaticClass(), false, FTransform::Identity, false);
	if (!Comp)
	{
		return;
	}

	USplineMeshComponent* SplineMesh = Cast<USplineMeshComponent>(Comp);
	if (!SplineMesh)
	{
		return;
	}

	SplineMesh->SetMobility(EComponentMobility::Movable);
	SplineMesh->SetStaticMesh(TargetMesh);
	UMaterialInterface* NewMaterial = bIsLigament ? (bIsHolding ? Cast<UMaterialInterface>(DynamicStretchedLigamentMaterial) : Cast<UMaterialInterface>(BaseLigamentMaterial)) : Cast<UMaterialInterface>(NerveMaterial);
	SplineMesh->SetMaterial(0, NewMaterial);

	SplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SplineMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SplineMesh->SetGenerateOverlapEvents(false);

	for (USplineMeshComponent* Mesh : SplineMeshes) if (Mesh) Mesh->SetMaterial(0, NewMaterial);

	SplineMesh->SetForwardAxis(CableForwardAxis, false);

	int Index = SplineMeshes.Num();
	float StartDistance = (Index * SingleCableLength);
	float EndDistance = ((Index + 1) * SingleCableLength);

	FVector StartSplineLocation = SplineCable->GetLocationAtDistanceAlongSpline(StartDistance, ESplineCoordinateSpace::Local);
	FVector EndSplineLocation = SplineCable->GetLocationAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::Local);

	FVector SplineDirection = UKismetMathLibrary::GetDirectionUnitVector(StartSplineLocation, EndSplineLocation);
	SplineMesh->SetStartAndEnd(StartSplineLocation, SplineDirection, EndSplineLocation, SplineDirection, false);

	SplineMesh->SetStartScale(CableScale, false);
	SplineMesh->SetEndScale(CableScale, false);

	SplineMesh->UpdateMesh();

	SplineMeshes.Add(SplineMesh);
}

void AENTNerve::RemoveSplineMesh()
{
	int LastIndex = SplineMeshes.Num() - 1;
	TObjectPtr<USplineMeshComponent> SplineMesh = SplineMeshes[LastIndex];

	SplineMeshes.RemoveAt(LastIndex);
	SplineMesh->DestroyComponent();
}

void AENTNerve::UpdateSplineMeshes(bool bUseNerveBallAsEndPoint, bool bMakeNoise)
{
	for (int i = 0; i < SplineMeshes.Num(); ++i)
	{
		TObjectPtr<USplineMeshComponent> SplineMesh = SplineMeshes[i];
		if (!SplineMesh)
		{
			continue;
		}

		float StartDistance = (i * SingleCableLength);
		float EndDistance = ((i + 1) * SingleCableLength);

		FVector StartSplineLocation = SplineCable->GetLocationAtDistanceAlongSpline(StartDistance, ESplineCoordinateSpace::Local);

		FVector EndSplineLocation;
		if ((i + 1) == SplineMeshes.Num() && bUseNerveBallAsEndPoint)
		{
			EndSplineLocation = NerveBall->GetRelativeLocation();
		}
		else
		{
			EndSplineLocation = SplineCable->GetLocationAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::Local);
		}

		SplineMesh->SetStartPosition(StartSplineLocation, false);
		SplineMesh->SetEndPosition(EndSplineLocation, false);

		SplineMesh->UpdateMesh();
	}

	int TargetNumberOfSplinesMeshes = FMath::CeilToInt((bUseNerveBallAsEndPoint ? GetNerveBallLength() : GetCableLength()) / SingleCableLength);

	if (TargetNumberOfSplinesMeshes == SplineMeshes.Num())
	{
		return;
	}

	if (TargetNumberOfSplinesMeshes > SplineMeshes.Num())
	{
		int NumberOfSplinesToAdd = TargetNumberOfSplinesMeshes - SplineMeshes.Num();
		for (int i = 0; i < NumberOfSplinesToAdd; i++)
		{
			AddSplineMesh(bMakeNoise);
		}
	}

	else if (TargetNumberOfSplinesMeshes < SplineMeshes.Num())
	{
		int NumberOfSplinesToRemove = SplineMeshes.Num() - TargetNumberOfSplinesMeshes;
		for (int i = 0; i < NumberOfSplinesToRemove; ++i)
		{
			RemoveSplineMesh();
		}
	}
}

void AENTNerve::BuildSplineMeshes()
{
	int NumberOfSplineToCreate = FMath::CeilToInt(StartCableLength / SingleCableLength);

	for (int i = 0; i < NumberOfSplineToCreate; i++)
	{
		AddSplineMesh(false);
	}
}

void AENTNerve::UpdateLastSplinePointLocation(const FVector& NewLocation)
{
	int32 LastSplinePointIndex = SplineCable->GetNumberOfSplinePoints() - 1;
	SplineCable->SetLocationAtSplinePoint(LastSplinePointIndex, NewLocation, ESplineCoordinateSpace::World);

	int LastSplineMeshIndex = (SplineMeshes.Num() - 1);
	if (!SplineMeshes.IsValidIndex(LastSplineMeshIndex))
	{
		return;
	}

	USplineMeshComponent* LastSplineMesh = SplineMeshes[LastSplineMeshIndex];

	if (!LastSplineMesh)
	{
		return;
	}

	FVector RelativeNewLocation = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), NewLocation);
	LastSplineMesh->SetEndPosition(RelativeNewLocation, false);

	FVector SplineDirection = UKismetMathLibrary::GetDirectionUnitVector(LastSplineMesh->GetStartPosition(), RelativeNewLocation);
	LastSplineMesh->SetStartTangent(SplineDirection, false);
	LastSplineMesh->SetEndTangent(SplineDirection, false);

	LastSplineMesh->UpdateMesh();
}

void AENTNerve::ApplyCablesPhysics()
{
	if (!bShouldApplyCablePhysics)
	{
		return;
	}

	if (SplineCable->GetNumberOfSplinePoints() < 2)
	{
		return;
	}

	float Alpha = UKismetMathLibrary::NormalizeToRange(GetCableLength(), 0.0f, CableMaxExtension);
	float RTPCValue = FMath::Lerp(0.0f, 100.0f, Alpha);

	UAkGameplayStatics::SetRTPCValue(NerveStretchRtpc, RTPCValue, 0, this);

	if (PlayerCharacter)
	{
		UENTCharacterStateMachine* StateMachine = PlayerCharacter->GetStateMachine();
		if (StateMachine)
		{
			if (StateMachine->GetCurrentStateID() == EENTCharacterStateID::Idle)
			{
				bIsStretchSoundPlayed = false;
				NerveStretchComp->Stop();
			}
			else if (!bIsStretchSoundPlayed)
			{
				bIsStretchSoundPlayed = true;
				NerveStretchComp->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
			}
		}
	}

	UpdateSplineMeshes(false, true);

	FRotator NerveBallRotator = FRotationMatrix::MakeFromX(GetCableDirection()).Rotator();
	NerveBallRotator += NerveBallRotationDelta;
	NerveBall->SetWorldRotation(NerveBallRotator);

	int32 LastPointIndex = SplineCable->GetNumberOfSplinePoints() - 2;

	FVector LastCableStartLocation = SplineCable->GetLocationAtSplinePoint(LastPointIndex, ESplineCoordinateSpace::World);
	FVector CableEndLocation = NerveBall->GetComponentLocation();

	UpdateLastSplinePointLocation(CableEndLocation);

	if (SplineCable->GetNumberOfSplineSegments() >= 2)
	{
		if (CanCurrentCableBeRemoved())
		{
			RemoveLastSplinePoint();
			return;
		}
	}

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	FHitResult Hit;

	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, LastCableStartLocation, CableEndLocation, CableColliders, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);

	if (!bHit)
	{
		return;
	}

#if WITH_EDITORONLY_DATA
	if (bDebugNerve && Hit.GetActor())
	{
		const FString Message = FString::Printf(TEXT("Hit: %s"), *Hit.GetActor()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, Message);
		FMessageLog("BlueprintLog").Message(EMessageSeverity::Info, FText::FromString(Message));
	}
#endif

	ImpactNormals.Add(Hit.Normal);

	FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(CableEndLocation, Hit.Location);

	bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, CableEndLocation, Hit.Location, CableColliders, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);
	if (!bHit)
	{
		return;
	}

	FVector WorldLocation = Hit.Location - (Direction * CableOffset);

	AddSplinePoint(WorldLocation, ESplineCoordinateSpace::World, true);
}

bool AENTNerve::CanCurrentCableBeRemoved()
{
	int32 CurrentPointIndex = SplineCable->GetNumberOfSplinePoints() - 1;
	int32 LastPointIndex = CurrentPointIndex - 1;

	FVector CurrentPointLocation = SplineCable->GetLocationAtSplinePoint(CurrentPointIndex, ESplineCoordinateSpace::World);
	FVector LastCableLocation = SplineCable->GetLocationAtSplinePoint(LastPointIndex, ESplineCoordinateSpace::World);

	FVector CurrentCableDirection = CurrentPointLocation - LastCableLocation;
	CurrentCableDirection.Normalize();

	int LastImpactNormalIndex = (ImpactNormals.Num() - 1);
	FVector LastImpactNormal = ImpactNormals[LastImpactNormalIndex];
	float DotResult = FVector::DotProduct((LastImpactNormal * -1), CurrentCableDirection);

	if (DotResult > 0.0f)
	{
		return false;
	}

	FHitResult Hit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	int32 OtherPointIndex = LastPointIndex - 1;
	FVector OtherPointLocation = SplineCable->GetLocationAtSplinePoint(OtherPointIndex, ESplineCoordinateSpace::World);
	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, OtherPointLocation, CurrentPointLocation, CableColliders, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);

	if (bHit)
	{
		return false;
	}

	ImpactNormals.RemoveAt(LastImpactNormalIndex);
	return true;
}

void AENTNerve::ResetCables(bool bHardReset)
{
	SplineCable->ClearSplinePoints(true);
	for (TObjectPtr<USplineMeshComponent> SplineMesh : SplineMeshes)
	{
		if (SplineMesh)
		{
			SplineMesh->DestroyComponent();
		}
	}

	SplineMeshes.Empty();

	ImpactNormals.Empty();

	if (bHardReset)
	{
		return;
	}

	FVector CableEndLocation = FVector(StartCableLength, 0.0f, 0.0f);

	AddSplinePoint(FVector::ZeroVector, ESplineCoordinateSpace::Local, false);
	AddSplinePoint(CableEndLocation, ESplineCoordinateSpace::Local, false);

	BuildSplineMeshes();

	NerveBall->SetRelativeLocation(CableEndLocation);
}

void AENTNerve::RetractCable(float Alpha)
{
	float AlphaRTPC = UKismetMathLibrary::NormalizeToRange(GetCableLength(), 0.0f, CableMaxExtension);
	float RTPCValue = FMath::Lerp(0.0f, 100.0f, AlphaRTPC);

	UAkGameplayStatics::SetRTPCValue(NerveStretchRtpc, RTPCValue, 0, this);

	float Distance = FMath::Lerp(SplineCable->GetDistanceAlongSplineAtSplinePoint(1), GetCableLength(), Alpha);
	FVector TargetLocation = SplineCable->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);

	NerveBall->SetRelativeLocation(TargetLocation);

	FVector CurrentSplineDirection = SplineCable->GetDirectionAtDistanceAlongSpline(GetNerveBallLength(), ESplineCoordinateSpace::World);
	CurrentSplineDirection *= -1;

	FRotator NerveBallRotator = FRotationMatrix::MakeFromX(CurrentSplineDirection).Rotator();
	NerveBallRotator += NerveBallRotationDelta;
	NerveBall->SetWorldRotation(NerveBallRotator);

	UpdateSplineMeshes(true, true);
}

void AENTNerve::FinishRetractCable()
{
	NerveBall->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	InteractableComponent->AddInteractable(NerveBall);
	if (!InteractableComponent->OnInteract.IsAlreadyBound(this, &AENTNerve::Interaction) && !bIsInWeakZone)
	{
		InteractableComponent->OnInteract.AddDynamic(this, &AENTNerve::Interaction);
	}

	bIsStretchSoundPlayed = false;
	NerveStretchComp->Stop();
	ResetCables(false);

	FRotator NerveBallRotator = FRotationMatrix::MakeFromY(GetCableDirection() * -1).Rotator();
	NerveBallRotator += NerveBallRotationDelta;
	NerveBall->SetWorldRotation(NerveBallRotator);
}



FVector AENTNerve::GetLastCableLocation(const ESplineCoordinateSpace::Type& CoordinateSpace) const
{
	int32 LastIndex = (SplineCable->GetNumberOfSplinePoints() - 1);
	return SplineCable->GetLocationAtSplinePoint(LastIndex, CoordinateSpace);
}

float AENTNerve::GetCableLength() const
{
	return SplineCable->GetSplineLength();
}

float AENTNerve::GetNerveBallLength() const
{
	if (!NerveBall)
	{
		return 0.0f;
	}

	return SplineCable->GetDistanceAlongSplineAtLocation(NerveBall->GetComponentLocation(), ESplineCoordinateSpace::World);
}

FVector AENTNerve::GetCableDirection() const
{
	if (SplineCable->GetNumberOfSplinePoints() < 2)
	{
		return FVector::ZeroVector;
	}

	int32 CurrentPointIndex = SplineCable->GetNumberOfSplinePoints() - 1;
	int32 LastPointIndex = CurrentPointIndex - 1;

	FVector CurrentPointLocation = SplineCable->GetLocationAtSplinePoint(CurrentPointIndex, ESplineCoordinateSpace::World);
	FVector LastPointLocation = SplineCable->GetLocationAtSplinePoint(LastPointIndex, ESplineCoordinateSpace::World);

	return UKismetMathLibrary::GetDirectionUnitVector(CurrentPointLocation, LastPointLocation);
}

FVector AENTNerve::GetCablePosition(float Percent, ESplineCoordinateSpace::Type CoordinateSpace) const
{
	float Distance = FMath::Lerp(0.0f, GetCableLength(), Percent);
	return SplineCable->GetLocationAtDistanceAlongSpline(Distance, CoordinateSpace);
}

void AENTNerve::ForceDetachNerveBallFromPlayer()
{
	if (!PlayerCharacter)
	{
		return;
	}

	UENTPhysicConstraint* Constraint = PlayerCharacter->GetComponentByClass<UENTPhysicConstraint>();
	if (Constraint)
	{
		Constraint->ReleasePlayer(true);
	}
	else
	{
		DetachNerveBall(true);
	}
}

#pragma endregion

#pragma region NerveBall

void AENTNerve::AttachNerveBall(AActor* ActorToAttach)
{
	NerveBall->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	bShouldApplyCablePhysics = true;

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);
	NerveBall->AttachToComponent(ActorToAttach->GetRootComponent(), Rules);
	NerveBall->SetRelativeLocation(GetDefault<UENTCoreConfig>()->PawnGrabObjectOffset);
}

void AENTNerve::DetachNerveBall(bool bForceDetachment)
{
	if (PlayerCharacter && PlayerCharacter->OnRespawn.IsAlreadyBound(this, &AENTNerve::ForceDetachNerveBallFromPlayer))
	{
		PlayerCharacter->GetStateMachine()->LockAllStates(false);
		PlayerCharacter->OnRespawn.RemoveDynamic(this, &AENTNerve::ForceDetachNerveBallFromPlayer);
	}

	PlayerCharacter = nullptr;
	PlayerController = nullptr;
	bShouldApplyCablePhysics = false;

	bIsHolding = false;

	FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, true);
	NerveBall->AttachToComponent(RootComponent, Rules);
	NerveBall->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SplineCable->AddSplinePointAtIndex(DefaultNervePosition, 1, ESplineCoordinateSpace::World, true);
	SplineCable->SetTangentAtSplinePoint(1, FVector::ZeroVector, ESplineCoordinateSpace::Local);

	RetractionIndex = SplineCable->GetNumberOfSplinePoints() - 2;

	if (bForceDetachment)
	{
		FinishRetractCable();
	}
	else
	{
		float RetractionDuration = GetCableLength() / RetractionSpeed;
		RetractTimeline.SetPlayRate(1/RetractionDuration);

		RetractTimeline.ReverseFromEnd();
	}
}

bool AENTNerve::IsNerveBallAttached() const
{
	return !NerveBall->IsAttachedTo(RootComponent);
}

#pragma endregion

#pragma region Interaction

void AENTNerve::Interaction(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent)
{
	if (CurrentAttachedReceptacle != nullptr)
	{
		if (!CurrentAttachedReceptacle->CanTheNerveBeTaken())
		{
			return;
		}

		CurrentAttachedReceptacle->DisableReceptacle();
		CurrentAttachedReceptacle->TriggerLinkedObjects(this);
		CurrentAttachedReceptacle = nullptr;
	}

	bIsLoaded = false;
	bIsHolding = true;

	UAkGameplayStatics::PostEventAtLocation(GrabNoise, NerveBall->GetComponentLocation(), NerveBall->GetComponentRotation(), this);

	PlayerController = Cast<AENTDefaultPlayerController>(Controller);
	AttachNerveBall(Pawn);

	AENTDefaultCharacter* Player = Cast<AENTDefaultCharacter>(Pawn);
	if (!Player)
	{
		return;
	}

	PhysicConstraint = Player->AddConstraint(bIsLigament);
	if (!PhysicConstraint)
	{
		return;
	}

	PhysicConstraint->Init(this, Player);
	
	InteractableComponent->RemoveInteractable(NerveBall);

	PlayerCharacter = Player;

	PlayerCharacter->GetStateMachine()->LockState(EENTCharacterStateID::Sprint, true);
	PlayerCharacter->GetStateMachine()->LockState(EENTCharacterStateID::Jump, true);

#if WITH_EDITOR
	if (PlayerCharacter->OnRespawn.IsAlreadyBound(this, &AENTNerve::ForceDetachNerveBallFromPlayer))
	{
		const FString Message = FString::Printf(TEXT("PlayerCharacter->OnRespawn is already bound, this will cause a freeze in a packaged game, please fix it"));

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
		return;
	}
#endif

	PlayerCharacter->OnRespawn.AddDynamic(this, &AENTNerve::ForceDetachNerveBallFromPlayer);
}

#pragma endregion

void AENTNerve::OnEnterWeakZone_Implementation(bool bIsZoneActive)
{
	IENTWeakZoneInterface::OnEnterWeakZone_Implementation(bIsZoneActive);

	bIsInWeakZone = true;

	if (!bIsLigament)
	{
		if (CurrentAttachedReceptacle != nullptr && bIsZoneActive)
		{
			if (!CurrentAttachedReceptacle->CanTheNerveBeTaken())
			{
				return;
			}

			CurrentAttachedReceptacle->DisableReceptacle();
			CurrentAttachedReceptacle->TriggerLinkedObjects(this);
			CurrentAttachedReceptacle = nullptr;

			DetachNerveBall(false);
		}
	}

	EnterWeakZoneTimeline.Play();

	if (bIsZoneActive && InteractableComponent->OnInteract.IsAlreadyBound(this, &AENTNerve::Interaction))
	{
		InteractableComponent->OnInteract.RemoveDynamic(this, &AENTNerve::Interaction);
	}
}

void AENTNerve::OnExitWeakZone_Implementation()
{
	bIsInWeakZone = false;
	IENTWeakZoneInterface::OnExitWeakZone_Implementation();
	
	EnterWeakZoneTimeline.Reverse();


	if (!InteractableComponent->OnInteract.IsAlreadyBound(this, &AENTNerve::Interaction))
	{
		InteractableComponent->OnInteract.AddDynamic(this, &AENTNerve::Interaction);
	}
}

void AENTNerve::UpdateEnterWeakZone(float Alpha)
{
	if (bIsLigament)
	{
		float ZOffset = (3.f * StartCableLength) / 4.f;
		FVector TargetPos = DefaultNervePosition + FVector(0.f, 0.f, ZOffset);
		FVector NewPos = FMath::Lerp(DefaultNervePosition, TargetPos, Alpha);
		NerveBall->SetWorldLocation(NewPos);
		UpdateSplineMeshes(true, false);
	}else
	{
		float Value = FMath::Lerp(0.7f, 0.45f, Alpha);
		CorruptMID->SetScalarParameterValue("Progress", Value);
	}
}

#pragma region Save

FENTGameElementData& AENTNerve::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	FENTNerveData Data;

	for (int32 i = 0; i < SplineCable->GetNumberOfSplinePoints(); i++)
	{
		FVector PointLocation = SplineCable->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);

		Data.SplinePointsLocations.Add(PointLocation);
	}

	Data.ImpactNormals = ImpactNormals;
	return CurrentWorldSave->NerveData.Add(GetName(), Data);
}

void AENTNerve::LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave)
{
	const FENTNerveData& Data = static_cast<const FENTNerveData&>(GameElementData);

	ResetCables(true);

	for (const FVector& SplinePointLocation : Data.SplinePointsLocations)
	{
		AddSplinePoint(SplinePointLocation, ESplineCoordinateSpace::Local, false);
	}

	FVector LastPointLocation = Data.SplinePointsLocations[Data.SplinePointsLocations.Num() - 1];
	NerveBall->SetRelativeLocation(LastPointLocation);

	UpdateSplineMeshes(false, false);

	ImpactNormals = Data.ImpactNormals;

	bIsLoaded = true;
}

#pragma endregion

void AENTNerve::SetCurrentReceptacle(AENTNerveReceptacle* Receptacle)
{
	CurrentAttachedReceptacle = Receptacle;

	if (!CurrentAttachedReceptacle)
	{
		return;
	}

	if (PlayerCharacter && PlayerCharacter->OnRespawn.IsAlreadyBound(this, &AENTNerve::ForceDetachNerveBallFromPlayer))
	{
		PlayerCharacter->GetStateMachine()->LockAllStates(false);
		PlayerCharacter->OnRespawn.RemoveDynamic(this, &AENTNerve::ForceDetachNerveBallFromPlayer);
	}

	PlayerCharacter = nullptr;
	PlayerController = nullptr;
	bShouldApplyCablePhysics = false;

	FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);
	NerveBall->AttachToComponent(RootComponent, Rules);

	FTransform AttachTransform = Receptacle->GetAttachTransform();
	NerveBall->SetWorldTransform(AttachTransform);

	bIsStretchSoundPlayed = false;
	NerveStretchComp->Stop();
	UpdateLastSplinePointLocation(AttachTransform.GetLocation());
	UpdateSplineMeshes(false, false);
	InteractableComponent->AddInteractable(NerveBall);
}
