// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NonDroneActor.h"
#include "JumpingKid.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKidAttackStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKidAttackEnd);

UCLASS()
class DRONERACER_QINGXIARUNTIME_API AJumpingKid : public ANonDroneActor
{
	GENERATED_BODY()

public:
	AJumpingKid(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly)
	bool bStartChasing;

	bool bGoingBack;

	UFUNCTION(BlueprintCallable)
	bool GetbStartChasing();

	UPROPERTY(BlueprintReadOnly)
	class UAIPerceptionComponent* AIVision;

	UPROPERTY(BlueprintReadOnly)
	class UAISenseConfig_Sight* VisionConfig;


	UPROPERTY(BlueprintAssignable)
	FOnKidAttackStart OnKidAttackStartDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnKidAttackEnd OnKidAttackEndDelegate;

	void JumpToDrone();

	UPROPERTY(BlueprintReadWrite)
	class APawn* Drone;

	FVector OrigLoc;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	virtual void PerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	void TakeAwardAway();

	void GoBackToLocation();

	UFUNCTION()
	void OnVisionDeactivated(UActorComponent* Component);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxChocolateDistance = 150.f;


	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float MaxDistanceTravelled = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovingSpeed= 50.f;
	
};
