// Fill out your copyright notice in the Description page of Project Settings.


#include "Parasite/ENTParasitePawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "ENTGravityPawnMovement.h"
#include "ENTHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Parasite/ENTParasiteController.h"
#include "Path/ENTArtificialIntelligencePath.h"
#include "Path/ENTNavigationArea.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTWorldSave.h"

#if WITH_EDITORONLY_DATA
#include "Selection.h"
#include "Components/ArrowComponent.h"
#endif

FVector AENTParasitePawn::DebugAttackLocation;
FVector AENTParasitePawn::DebugAttackSize;

AENTParasitePawn::AENTParasitePawn()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ParasiteCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ParasiteHitBox"));
	SetRootComponent(ParasiteCollision);

	ParasiteCollision->SetCollisionObjectType(ECC_Pawn);
	ParasiteCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ParasiteCollision->SetCollisionResponseToAllChannels(ECR_Block);

	ParasiteMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	ParasiteMesh->SetupAttachment(ParasiteCollision);

#if WITH_EDITORONLY_DATA
	ForwardDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardDirection"));
	ForwardDirection->SetupAttachment(ParasiteCollision);
	ForwardDirection->bIsEditorOnly = true;

	LeftDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftDirection"));
	LeftDirection->SetupAttachment(ParasiteCollision);
	LeftDirection->bIsEditorOnly = true;
	LeftDirection->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	LeftDirection->SetArrowColor(FLinearColor::Green);

	UpDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("UpDirection"));
	UpDirection->SetupAttachment(ParasiteCollision);
	UpDirection->bIsEditorOnly = true;
	UpDirection->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	UpDirection->SetArrowColor(FLinearColor::Blue);
#endif

	ParasiteDeathZone = CreateDefaultSubobject<UBoxComponent>(TEXT("ParasiteDeathZone"));
	ParasiteDeathZone->SetupAttachment(ParasiteMesh);
	ParasiteDeathZone->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	MovementComponent = CreateDefaultSubobject<UENTGravityPawnMovement>(TEXT("Movement"));
	MovementComponent->MaxSpeed = 400.0f;

	AIControllerClass = AENTParasiteController::StaticClass();

#if WITH_EDITORONLY_DATA
	USelection::SelectObjectEvent.AddUObject(this, &AENTParasitePawn::OnSelectionUpdate);
	USelection::SelectionChangedEvent.AddUObject(this, &AENTParasitePawn::OnSelectionUpdate);
#endif
}

void AENTParasitePawn::BeginPlay()
{
	Super::BeginPlay();

	if (!ParasiteController)
	{
		return;
	}

	ParasiteDeathZone->OnComponentBeginOverlap.AddDynamic(this, &AENTParasitePawn::EnterDeathZone);

	if (TargetPath && !TargetPath->IsOnFloor())
	{
		MovementComponent->SetGravityScale(0.0f);
	}
}

void AENTParasitePawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!ParasiteDeathZone)
	{
		return;
	}

	if (ParasiteDeathZone->OnComponentBeginOverlap.IsAlreadyBound(this, &AENTParasitePawn::EnterDeathZone))
	{
		ParasiteDeathZone->OnComponentBeginOverlap.RemoveDynamic(this, &AENTParasitePawn::EnterDeathZone);
	}
}

void AENTParasitePawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!ParasiteCollision)
	{
		return;
	}

#if WITH_EDITORONLY_DATA
	if (SelectedInEditor)
	{
		DrawDetectionRange();
	}
#endif

	if (!bOverrideDefaultRotation)
	{
		FRotator ActorRotation = GetActorRotation();
		SetActorRotation(FRotator(-90.0f, ActorRotation.Yaw, 0.0f));
	}

	if (!TargetPath)
	{
		return;
	}

#if WITH_EDITORONLY_DATA
	bool bIsAttached = TargetPath->AttachAI(this);

	if (!bIsAttached)
	{
		return;
	}
#endif

	FHitResult HitResult;
	bool bHit = TargetPath->GetTracedPointLocation(0, HitResult);

	if (!bHit)
	{
		return;
	}

	FVector ActorLocation = HitResult.Location;
	ActorLocation += (TargetPath->GetDirection() * -1 * GetHitBoxHeight());
	SetActorLocation(ActorLocation);

	FRotator Rotation = UKismetMathLibrary::MakeRotFromZ(HitResult.Normal);
	SetActorRotation(Rotation);
}

#if WITH_EDITOR
void AENTParasitePawn::PostLoad()
{
	Super::PostLoad();

	ParasiteHeight = GetHitBoxHeight();
	ParasiteWidth = GetHitBoxWidth();

	DebugAttackLocation = AttackLocation;
	DebugAttackSize = AttackSize;
}

void AENTParasitePawn::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	if (!PropertyAboutToChange)
	{
		return;
	}

	if (PropertyAboutToChange->NamePrivate == GET_MEMBER_NAME_CHECKED(AENTParasitePawn, TargetPath))
	{
		if (TargetPath)
		{
			TargetPath->DetachAI(this);
		}
	}
}

void AENTParasitePawn::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName& ChangedProperty = PropertyChangedEvent.GetMemberPropertyName();

	if (ChangedProperty == GET_MEMBER_NAME_CHECKED(AENTParasitePawn, TargetPath))
	{
		if (TargetPath)
		{
			NavigationArea = nullptr;
			bool bIsAttached = TargetPath->AttachAI(this);

			if (!bIsAttached)
			{
				TargetPath = nullptr;
			}
		}
	}
	else if (ChangedProperty == GET_MEMBER_NAME_CHECKED(AENTParasitePawn, NavigationArea))
	{
		if (NavigationArea)
		{
			TargetPath = nullptr;
		}
	}
	else if (ChangedProperty == GET_MEMBER_NAME_CHECKED(AENTParasitePawn, ParasiteCollision))
	{
		ParasiteHeight = GetHitBoxHeight();
		ParasiteWidth = GetHitBoxWidth();
	}
	else if (ChangedProperty == GET_MEMBER_NAME_CHECKED(AENTParasitePawn, AttackLocation))
	{
		DebugAttackLocation = AttackLocation;
	}
	else if (ChangedProperty == GET_MEMBER_NAME_CHECKED(AENTParasitePawn, AttackSize))
	{
		DebugAttackSize = AttackSize;
	}
}
#endif

void AENTParasitePawn::OnBehaviorTreeStarted_Implementation()
{
	IENTPawnAIInterface::OnBehaviorTreeStarted_Implementation();

	if (!ParasiteController->GetBlackboardComponent())
	{
		return;
	}

	if (TargetPath)
	{
		ParasiteController->GetBlackboardComponent()->SetValueAsObject(PathKeyName, TargetPath);
		ParasiteController->GetBlackboardComponent()->SetValueAsBool(WalkOnFloorKeyName, TargetPath->IsOnFloor());
	}

	if (NavigationArea)
	{
		ParasiteController->GetBlackboardComponent()->SetValueAsObject(NavAreaKeyName, NavigationArea);
	}

	ParasiteController->GetBlackboardComponent()->SetValueAsFloat(PatrolSpeedKeyName, PatrolSpeed);
	ParasiteController->GetBlackboardComponent()->SetValueAsFloat(ChaseSpeedKeyName, ChaseSpeed);

	ParasiteController->GetBlackboardComponent()->SetValueAsFloat(DetectionRangeKeyName, DefaultDetectionRange);
}

void AENTParasitePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ParasiteController = Cast<AENTParasiteController>(NewController);

#if WITH_EDITOR
	if (!ParasiteController)
	{
		const FString Message = FString::Printf(TEXT("%s has wrong AIController, must be %s"), *GetClass()->GetName(), *AENTParasiteController::StaticClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
	}
#endif
}

#pragma region DetectionRange

#if WITH_EDITOR
void AENTParasitePawn::DrawDetectionRange() const
{
	ClearDebugTraces();

	if (ParasiteController && ParasiteController->GetBlackboardComponent())
	{
		float CurrentRadius = ParasiteController->GetBlackboardComponent()->GetValueAsFloat(DetectionRangeKeyName);

		UKismetSystemLibrary::DrawDebugCylinder(this, GetActorLocation(), GetActorLocation(), CurrentRadius, 12, FLinearColor::Red, INFINITY, 10.0f);
	}
	else
	{
		UKismetSystemLibrary::DrawDebugCylinder(this, GetActorLocation(), GetActorLocation(), DefaultDetectionRange, 12, FLinearColor::Red, INFINITY, 10.0f);
		UKismetSystemLibrary::DrawDebugCylinder(this, GetActorLocation(), GetActorLocation(), AugmentedDetectionRange, 12, FLinearColor::Yellow, INFINITY, 10.0f);
	}
}
#endif

void AENTParasitePawn::ChangeDetectionRange(bool bDoesPlayerHaveAmber)
{
	ParasiteController->GetBlackboardComponent()->SetValueAsFloat(DetectionRangeKeyName, bDoesPlayerHaveAmber ? AugmentedDetectionRange : DefaultDetectionRange);
}

#pragma endregion

#pragma region ParasiteAttack

void AENTParasitePawn::Attack()
{
	TArray<AActor*> Actors;

	ParasiteDeathZone->GetOverlappingActors(Actors, AActor::StaticClass());

	bool bHitSomething = false;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	TArray<FHitResult> HitResults;
	bool bHit = UKismetSystemLibrary::BoxTraceMultiForObjects(this, AttackLocation, AttackLocation, AttackSize, FRotator::ZeroRotator, ObjectsToAttack, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResults, true);

	if (!bHit)
	{
		// failed attack
		return;
	}

	for (const FHitResult& HitResult : HitResults)
	{
		AActor* Actor = HitResult.GetActor();
		if (!Actor)
		{
			continue;
		}

		UENTHealthComponent* HealthComponent = Actor->GetComponentByClass<UENTHealthComponent>();
		if (!HealthComponent)
		{
			continue;
		}

		// succeed attack
		HealthComponent->TakeDamages(AttackDamages);
		bHitSomething = true;
	}

	if (!bHitSomething)
	{
		return;
	}
}

void AENTParasitePawn::EnterDeathZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ParasiteController || !ParasiteController->GetBlackboardComponent() || !OtherActor)
	{
		return;
	}

	ParasiteController->GetBlackboardComponent()->SetValueAsObject(AttackTargetKeyName, OtherActor);
}

void AENTParasitePawn::DebugAttackZone(const UObject* WorldContextObject)
{
#if WITH_EDITORONLY_DATA
	UKismetSystemLibrary::DrawDebugBox(WorldContextObject, DebugAttackLocation, DebugAttackSize, FLinearColor::Red);
#endif
}

#pragma endregion

#if WITH_EDITORONLY_DATA
void AENTParasitePawn::DebugPawn() const
{
	if (!ParasiteController || !ParasiteController->GetBlackboardComponent())
	{
		return;
	}

	UObject* PathValue = ParasiteController->GetBlackboardComponent()->GetValueAsObject(PathKeyName);
	FString PathValueName = "Nullptr";
	if (PathValue)
	{
		PathValueName = PathValue->GetName();
	}
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("%s: %s"), *PathKeyName.ToString(), *PathValueName));

	UObject* NavAreaValue = ParasiteController->GetBlackboardComponent()->GetValueAsObject(NavAreaKeyName);
	FString NavAreaValueName = "Nullptr";
	if (NavAreaValue)
	{
		NavAreaValueName = NavAreaValue->GetName();
	}
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("%s: %s"), *NavAreaKeyName.ToString(), *NavAreaValueName));


	bool bWalkOnFloor = ParasiteController->GetBlackboardComponent()->GetValueAsBool(WalkOnFloorKeyName);
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("%s: %s"), *WalkOnFloorKeyName.ToString(), (bWalkOnFloor ? TEXT("true") : TEXT("false"))));

	UObject* AttackTargetValue = ParasiteController->GetBlackboardComponent()->GetValueAsObject(AttackTargetKeyName);
	FString AttackTargetValueName = "Nullptr";
	if (AttackTargetValue)
	{
		AttackTargetValueName = PathValue->GetName();
	}
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("%s: %s"), *AttackTargetKeyName.ToString(), *AttackTargetValueName));
}
#endif

#pragma region MathFunctions

float AENTParasitePawn::GetHitBoxHeight() const
{
	if (ParasiteCollision)
	{
		// due to the capsule rotation the height of the parasite is the capsule radius;
		return ParasiteCollision->GetUnscaledCapsuleRadius();
	}

	return 0.0f;
}

float AENTParasitePawn::GetHitBoxWidth() const
{
	if (ParasiteCollision)
	{
		// due to the capsule rotation the width of the parasite is the capsule halfHeight;
		return ParasiteCollision->GetUnscaledCapsuleHalfHeight();
	}

	return 0.0f;
}

FVector AENTParasitePawn::GetParasiteForwardVector() const
{
	if(ParasiteMesh)
	{
		return ParasiteMesh->GetForwardVector();
	}

	return FVector::ZeroVector;
}

FVector AENTParasitePawn::GetParasiteRightVector() const
{
	if(ParasiteMesh)
	{
		return ParasiteMesh->GetRightVector();
	}

	return FVector::ZeroVector;
}

FVector AENTParasitePawn::GetParasiteUpVector() const
{
	if(ParasiteMesh)
	{
		return ParasiteMesh->GetUpVector();
	}

	return FVector::ZeroVector;
}

#pragma endregion

void AENTParasitePawn::SetAnimToTrigger(UAnimSequenceBase* Anim)
{
	AnimToTrigger = Anim;
	OnChangeAnimToTrigger.Broadcast(AnimToTrigger);
}

#pragma region Velocity

FVector AENTParasitePawn::GetVelocity() const
{
	if (bOverrideVelocity)
	{
		return OverridenVelocity;
	}

	return Super::GetVelocity();
}

void AENTParasitePawn::OverrideVelocity(bool bOverride)
{
	OverrideVelocity(bOverride ? FVector::ForwardVector : FVector::ZeroVector);
}

void AENTParasitePawn::OverrideVelocity(const FVector& NewVelocity)
{
	bOverrideVelocity = NewVelocity != FVector::ZeroVector;
	OverridenVelocity = NewVelocity;
}

#pragma endregion

#pragma region Save

FENTGameElementData& AENTParasitePawn::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	FENTParasiteData Data;
	Data.PawnTransform = GetActorTransform();

	if (ParasiteController)
	{
		ParasiteController->SaveControllerData(Data);
	}

	return CurrentWorldSave->ParasiteData.Add(GetName(), Data);
}

void AENTParasitePawn::LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave)
{
	const FENTParasiteData& Data = static_cast<const FENTParasiteData&>(GameElementData);

	SetActorTransform(Data.PawnTransform);

	bHasReceivedLoadingRequest = true;
	LoadingData = Data;
}

#pragma endregion

#pragma region DebugSelection

void AENTParasitePawn::OnSelectionUpdate(UObject* Object)
{
	if (Object == this && !SelectedInEditor)
	{
		SelectedInEditor = true;
		DrawDetectionRange();
	}
	else if (SelectedInEditor && !IsSelected())
	{
		SelectedInEditor = false;
		ClearDebugTraces();
	}
}

void AENTParasitePawn::ClearDebugTraces() const
{
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
}

#pragma endregion
