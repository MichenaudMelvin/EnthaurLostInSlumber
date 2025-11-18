// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ENTJumpLink.generated.h"

class UNavLinkCustomComponent;

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API AENTJumpLink : public AActor
{
	GENERATED_BODY()

public:
	AENTJumpLink();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NavLink")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NavLink")
	TObjectPtr<UNavLinkCustomComponent> NavLink;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "NavLink", meta = (MakeEditWidget))
	FVector FirstNavLinkLocation = FVector::ZeroVector;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "NavLink", meta = (MakeEditWidget))
	FVector SecondNavLinkLocation = FVector(500.0f, 0.0f, 0.0f);

	/**
	 * @brief Blackboard key for AI
	 */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName JumpKeyName = "RequestJump";

	/**
	 * @brief Blackboard key for AI
	 */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName JumpLocationKeyName = "JumpLocation";

	void NotifyLinkReached(UNavLinkCustomComponent* NavLinkCustomComponent, UObject* PathingAgent, const UE::Math::TVector<double>& Destination);

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Transient, Category = "NavLink")
	bool bIgnoreGroundTrace = false;

	UPROPERTY(EditAnywhere, Category = "NavLink", meta = (Units = cm, ClampMin = 0.0f))
	float GroundTraceLength = 500.0f;

	UPROPERTY(Transient)
	TObjectPtr<class UArrowComponent> FistNavLinkDebugArrow;

	UPROPERTY(Transient)
	TObjectPtr<class UArrowComponent> SecondNavLinkDebugArrow;
#endif
};
