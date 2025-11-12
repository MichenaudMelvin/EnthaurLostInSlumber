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
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "Ground", meta = (ClampMin = 0.0f, Units = "cm"))
	float GroundTraceLength = 150.0f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDebugTask = false;
#endif
};
