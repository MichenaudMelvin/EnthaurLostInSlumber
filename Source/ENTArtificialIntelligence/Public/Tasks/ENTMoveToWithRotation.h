// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "ENTMoveToWithRotation.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTMoveToWithRotation : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UENTMoveToWithRotation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Ground", meta = (ClampMin = 0.0f, Units = cm))
	float GroundTraceLength = 150.0f;

	UPROPERTY(EditInstanceOnly, Category = "Ground")
	FRotator RotationOffset;

	UPROPERTY(EditInstanceOnly, Category = "Ground")
	bool bLerpRotation = true;

	UPROPERTY(EditInstanceOnly, Category = "Ground", meta = (Units = s, EditCondition = bLerpRotation))
	float RotationSpeed = 5.0f;

	FNavigationPath* CurrentPath;

	int32 CurrentPathIndex = -1;

	FVector CurrentDirection = FVector::ZeroVector;

	bool bRequestDirectionUpdate = false;

	void OnUpdatePath(FNavigationPath* InPath, ENavPathEvent::Type Event);

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDebugTask = false;

	UPROPERTY(EditInstanceOnly, Category = "Debug", meta = (Units = cm))
	float LineLength = 200.0f;

	/**
	 * @brief This is for debug purpose only
	 */
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDisableRotation = false;
#endif
};
