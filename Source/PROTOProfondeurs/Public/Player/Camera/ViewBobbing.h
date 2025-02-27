// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "ViewBobbing.generated.h"

UCLASS(Abstract)
class PROTOPROFONDEURS_API UViewBobbing : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UViewBobbing(const FObjectInitializer& ObjectInitializer);
};
