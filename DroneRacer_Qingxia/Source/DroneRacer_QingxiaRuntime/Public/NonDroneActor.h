// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LyraCharacter.h"
#include "NonDroneActor.generated.h"

/**
 * 
 */
UCLASS()
class DRONERACER_QINGXIARUNTIME_API ANonDroneActor : public ALyraCharacter
{
	GENERATED_BODY()

public:

	ANonDroneActor(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;


};
