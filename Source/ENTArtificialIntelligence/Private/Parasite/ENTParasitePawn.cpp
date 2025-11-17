// Fill out your copyright notice in the Description page of Project Settings.


#include "Parasite/ENTParasitePawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "ENTGravityPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "Parasite/ENTParasiteController.h"
#include "Path/ENTArtificialIntelligencePath.h"
#include "Path/ENTNavigationArea.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTWorldSave.h"

#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#endif

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

void AENTParasitePawn::EnterDeathZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ParasiteController || !ParasiteController->GetBlackboardComponent() || !OtherActor)
	{
		return;
	}

	ParasiteController->GetBlackboardComponent()->SetValueAsObject(AttackTargetKeyName, OtherActor);
}

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