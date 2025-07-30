// Fill out your copyright notice in the Description page of Project Settings.


#include "Parasite/ENTParasitePawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "ENTGravityPawnMovement.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "Parasite/ENTParasiteController.h"
#include "Path/ENTAIPath.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTWorldSave.h"

#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#endif

AENTParasitePawn::AENTParasitePawn()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ParasiteCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ParasiteHitBox"));
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

	if (TargetPath && ParasiteController->GetBlackboardComponent())
	{
		ParasiteController->GetBlackboardComponent()->SetValueAsObject(PathKeyName, TargetPath);
		ParasiteController->GetBlackboardComponent()->SetValueAsBool(WalkOnFloorKeyName, TargetPath->IsOnFloor());

		if (!TargetPath->IsOnFloor())
		{
			MovementComponent->SetGravityScale(0.0f);
		}
	}

	if (bLoadBlackboardData)
	{
		ParasiteController->LoadBlackboardValues(BlackboardData);
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
	ActorLocation += (TargetPath->GetDirection() * -1 * ParasiteCollision->GetUnscaledBoxExtent().Z);
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
			bool bIsAttached = TargetPath->AttachAI(this);

			if (!bIsAttached)
			{
				TargetPath = nullptr;
			}
		}
	}
}
#endif

#pragma region Save

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

FENTGameElementData& AENTParasitePawn::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	FENTParaSiteData Data;
	Data.PawnTransform = GetActorTransform();

	if (ParasiteController)
	{
		ParasiteController->SaveBlackBoardValues(Data);
	}

	return CurrentWorldSave->ParasiteData.Add(GetName(), Data);
}

void AENTParasitePawn::LoadGameElement(const FENTGameElementData& GameElementData)
{
	const FENTParaSiteData& Data = static_cast<const FENTParaSiteData&>(GameElementData);

	SetActorTransform(Data.PawnTransform);

	bLoadBlackboardData = true;
	BlackboardData = Data;
}

#pragma endregion