// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/ENTTeleportationVein.h"
#include "ENTInteractableComponent.h"

AENTTeleportationVein::AENTTeleportationVein()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);
}

