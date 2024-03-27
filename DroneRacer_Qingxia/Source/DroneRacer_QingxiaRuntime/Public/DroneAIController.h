// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularAIController.h"
#include "DroneAIController.generated.h"

/**
 * 
 */
UCLASS()
class DRONERACER_QINGXIARUNTIME_API ADroneAIController : public AModularAIController
{
	GENERATED_BODY()

public:
	ADroneAIController(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAIPerceptionComponent* AIVision;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	class UAISenseConfig_Sight* VisionConfig;

	UPROPERTY(BlueprintReadWrite)
	AActor* TargetActor;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<AActor> TargetActorType;

	UFUNCTION()
	void PerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void WhenPerceptionForget(AActor* Actor);

	
};
