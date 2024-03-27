// Fill out your copyright notice in the Description page of Project Settings.


#include "NonDroneActor.h"

ANonDroneActor::ANonDroneActor(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ANonDroneActor::BeginPlay()
{
	Super::BeginPlay();
}
