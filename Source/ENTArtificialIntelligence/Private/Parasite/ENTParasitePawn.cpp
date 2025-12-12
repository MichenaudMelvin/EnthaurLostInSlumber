// Fill out your copyright notice in the Description page of Project Settings.


#include "Parasite/ENTParasitePawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ENTGravityPawnMovement.h"
#include "ENTHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Parasite/ENTParasiteController.h"
#include "Path/ENTArtificialIntelligencePath.h"
#include "Path/ENTNavigationArea.h"
#include "Player/ENTDefaultCharacter.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTWorldSave.h"
#include "Subsystems/ENTWorldSaveSubsystem.h"

#if WITH_EDITORONLY_DATA
#include "Selection.h"
#include "Components/ArrowComponent.h"
#endif

AENTParasitePawn::AENTParasitePawn()
{
	PrimaryActorTick.bCanEverTick = false; // tick is called by the AISubsystem
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

	if (TargetPath && !TargetPath->IsOnFloor())
	{
		MovementComponent->SetGravityScale(0.0f);
	}
}

#if WITH_EDITOR
void AENTParasitePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDebugDetectionRange)
	{
		DrawDetectionRange();
	}
}
#endif

void AENTParasitePawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	AActor* ResultActor = UGameplayStatics::GetActorOfClass(this, AENTDefaultCharacter::StaticClass());

	if (ResultActor)
	{
		AENTDefaultCharacter* Character = Cast<AENTDefaultCharacter>(ResultActor);
		if (Character)
		{
			Character->OnAmberUpdate.RemoveDynamic(this, &AENTParasitePawn::ChangeDetectionRange);
		}
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

	FTransform TargetTransform = TargetPath->GetStartTransform(1);
	FQuat Rotation = TargetTransform.GetRotation() * FRotator(-90.0f, 0.0f, 0.0f).Quaternion();
	TargetTransform.SetRotation(Rotation);

	SetActorTransform(TargetTransform);
}

#if WITH_EDITOR
void AENTParasitePawn::PostLoad()
{
	Super::PostLoad();

	ParasiteHeight = GetParasiteHeight();
	ParasiteWidth = GetParasiteWidth();
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
		ParasiteHeight = GetParasiteHeight();
		ParasiteWidth = GetParasiteWidth();
	}
}
#endif

void AENTParasitePawn::FellOutOfWorld(const UDamageType& dmgType)
{
	// Super::FellOutOfWorld(dmgType);

	if (bAllowRespawn)
	{
		RespawnParasite();
	}
	else
	{
		Destroy();
	}
}

void AENTParasitePawn::RespawnParasite()
{
	if (!bAllowRespawn)
	{
		return;
	}

	if (!ParasiteController)
	{
		return;
	}

	if (!ParasiteController->GetBlackboardComponent())
	{
		return;
	}

	FVector SpawnLocation = ParasiteController->GetBlackboardComponent()->GetValueAsVector(SpawnLocationKeyName);
	FRotator SpawnRotation = ParasiteController->GetBlackboardComponent()->GetValueAsRotator(SpawnRotationKeyName);

	SetActorLocation(SpawnLocation);
	SetActorRotation(SpawnRotation);

	ParasiteController->GetBlackboardComponent()->SetValueAsBool(ResetBehaviorKeyName, true);
}

#pragma region BehaviorTree

void AENTParasitePawn::OnBehaviorTreeStarted_Implementation()
{
	IENTPawnAIInterface::OnBehaviorTreeStarted_Implementation();

	if (!ParasiteController->GetBlackboardComponent())
	{
		return;
	}

	ParasiteController->GetBlackboardComponent()->SetValueAsBool(UseNavMeshKeyName, bUseNavMesh);

	if (TargetPath)
	{
		ParasiteController->GetBlackboardComponent()->SetValueAsObject(PathKeyName, TargetPath);
		ParasiteController->GetBlackboardComponent()->SetValueAsBool(WalkOnFloorKeyName, TargetPath->IsOnFloor());
	}

	if (NavigationArea)
	{
		ParasiteController->GetBlackboardComponent()->SetValueAsObject(NavAreaKeyName, NavigationArea);
	}

	MovementComponent->MaxSpeed = PatrolSpeed;
	ParasiteController->GetBlackboardComponent()->SetValueAsFloat(PatrolSpeedKeyName, PatrolSpeed);
	ParasiteController->GetBlackboardComponent()->SetValueAsFloat(ChaseSpeedKeyName, ChaseSpeed);

	ParasiteController->GetBlackboardComponent()->SetValueAsFloat(DetectionRangeKeyName, DefaultDetectionRange);

	AActor* ResultActor = UGameplayStatics::GetActorOfClass(this, AENTDefaultCharacter::StaticClass());

	if (ResultActor)
	{
		AENTDefaultCharacter* Character = Cast<AENTDefaultCharacter>(ResultActor);
		if (Character)
		{
			Character->OnAmberUpdate.AddDynamic(this, &AENTParasitePawn::ChangeDetectionRange);
			Character->OnRespawn.AddDynamic(this, &AENTParasitePawn::RespawnParasite);
			ParasiteController->GetBlackboardComponent()->SetValueAsObject(PlayerKeyName, Character);
		}
	}
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

void AENTParasitePawn::StartBehaviorTree()
{
	if (!ParasiteController)
	{
		return;
	}

	ParasiteController->RunCurrentBehaviorTree();
}

void AENTParasitePawn::Trigger_Implementation()
{
	IENTActivation::Trigger_Implementation();

	StartBehaviorTree();
}

#pragma endregion

#pragma region DetectionRange

#if WITH_EDITORONLY_DATA
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
	ParasiteController->GetBlackboardComponent()->SetValueAsBool(DoesPlayerHaveAmberKeyName, bDoesPlayerHaveAmber);
}

#pragma endregion

#pragma region ParasiteAttack

void AENTParasitePawn::QueryForAttack(const FVector& AttackLocation, const FVector& AttackExtent)
{
	FTransform RelativeTransform = FTransform(FRotator::ZeroRotator, AttackLocation, FVector::OneVector);
	FTransform WorldTransform = RelativeTransform * ParasiteMesh->GetComponentTransform();

#if WITH_EDITORONLY_DATA
	if (bDebugAttack)
	{
		DebugAttackZone(this, WorldTransform.GetLocation(), AttackExtent, WorldTransform.GetRotation().Rotator(),15.0f);
	}
#endif

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	TArray<FHitResult> HitResults;
	bool bHit = UKismetSystemLibrary::BoxTraceMultiForObjects(this, WorldTransform.GetLocation(), WorldTransform.GetLocation(), AttackExtent, WorldTransform.GetRotation().Rotator(), ObjectsToAttack, false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true);
	if (!bHit)
	{
		// failed attack
		return;
	}

	FoundedHealthComp.Empty();
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

		FoundedHealthComp.Add(HealthComponent);
	}
}

void AENTParasitePawn::Attack()
{
	if (FoundedHealthComp.IsEmpty())
	{
		// failed attack
		return;
	}

	for (TObjectPtr<UENTHealthComponent> HealthComp : FoundedHealthComp)
	{
		HealthComp->TakeDamages(AttackDamages);
	}

	FoundedHealthComp.Empty();
}

bool AENTParasitePawn::SucceedAttack() const
{
	return !FoundedHealthComp.IsEmpty();
}

void AENTParasitePawn::DebugAttackZone(const UObject* WorldContextObject, const FVector& AttackLocation, const FVector& AttackExtent, const FRotator& Rotation, float Duration)
{
#if WITH_EDITORONLY_DATA
	UKismetSystemLibrary::DrawDebugBox(WorldContextObject, AttackLocation, AttackExtent, FLinearColor::Red, Rotation, Duration);
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

float AENTParasitePawn::GetParasiteHeight() const
{
	if (ParasiteCollision)
	{
		// due to the capsule rotation the height of the parasite is the capsule radius;
		return ParasiteCollision->GetUnscaledCapsuleRadius() * 2;
	}

	return 0.0f;
}

float AENTParasitePawn::GetParasiteWidth() const
{
	if (ParasiteCollision)
	{
		// due to the capsule rotation the width of the parasite is the capsule halfHeight;
		return ParasiteCollision->GetUnscaledCapsuleHalfHeight() * 2;
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

	UENTWorldSaveSubsystem* WorldSaveSubsystem = GetGameInstance()->GetSubsystem<UENTWorldSaveSubsystem>();
	if(!WorldSaveSubsystem || !ParasiteController)
	{
		return;
	}

	WorldSaveSubsystem->OnFinishLoading.AddDynamic(ParasiteController, &AENTParasiteController::LoadingActions);
}

#pragma endregion

#pragma region DebugSelection

#if WITH_EDITORONLY_DATA
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
#endif

#pragma endregion
